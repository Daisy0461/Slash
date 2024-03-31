#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Enemy/EnemyCombat.h"
#include "Character/VikingCharacter.h"
#include "Components/AttributeComponent.h"
#include "Animation/AnimInstance.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "Item/Health.h"
#include "UObject/Class.h"
#include "Character/DamageCameraShake.h"
#include "Kismet/GameplayStatics.h"



AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Components 추가
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	EnemyMove = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("EnemyMoveComponent"));
	EnemyCombat = CreateDefaultSubobject<UEnemyCombat>(TEXT("EnemyCombatComponent"));
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GetRootComponent());
	PawnSensing->SightRadius = 45.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsDead() && CombatTarget) return;

	if(EnemyState > EEnemyState::EES_Patrolling){
		CheckCombatTarget();
	}else{
		EnemyMove->CheckPatrolTarget();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Weapon & Shield 장착
	UWorld* World = GetWorld();
	if(World && WeaponClass){
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		AShield* DefaultShield = World->SpawnActor<AShield>(ShieldClass);
		if(DefaultWeapon){
			DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
			EquippedWeapon = DefaultWeapon;
		}
		if(DefaultShield){
			DefaultShield->Equip(GetMesh(), FName("LeftHandSocket"), this, this);
		}
	}

	//HealthBarWiget 최초에 숨기기
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}

	if(PawnSensing){
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	Tags.Add(FName("Enemy"));
}

void AEnemy::Die()
{
	Super::Die();
	
	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	//죽은 후 Collision 없애기
	DisableCapsuleCollision();
	SetWeaponCollision(ECollisionEnabled::NoCollision);
	HideHealthBar();
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(DestoryTime);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//Note: 랜덤적으로 Spawn하게 하면 좋을 듯 
	SpawnHealItem();
}

void AEnemy::Attack()
{
	Super::Attack();
	if(CombatTarget == nullptr) return;

	if(AutoAttackMontage && IsInSideAutoAttackRadius()){
		//UE_LOG(LogTemp, Display, TEXT("AA Radius"));
		PlayAutoAttackMontage();
	}else if(MotionWarpAttackMontage && IsInSideMotionWarpAttackRadius()){
		//UE_LOG(LogTemp, Display, TEXT("Non AA Radius"));
		PlayMotionWarpAttackMontage();
	}
	
	EnemyState = EEnemyState::EES_Engaged;
}

void AEnemy::SpawnFireBall()
{
	if(!FireBallActor) return;

	//아예 플레이어와의 거리를 재고 일정거리 이상이면 멀어지는 건 어때?? 그래 그러자
	//마법사는 Raidus가 더 길고 공격 범위도 긴걸로 하자.
	if(GetWorld()){
		//auto FireBall = 
		//
		GetWorld()->SpawnActor<AActor>(
		FireBallActor,
		ProjectileSpawnPoint->GetComponentLocation(), 
		ProjectileSpawnPoint->GetComponentRotation());
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Can't Cast FireBall"));
	}
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::StartHitStop(float DamageAmount, AActor* PlayerActor)
{	

	CustomTimeDilation = 0.0f;
	PlayerActor->CustomTimeDilation = 0.0f;
	float HitStopTime = DamageAmount * HitStopModifier;

	if(GetWorld()){
		//UE_LOG(LogTemp, Display, TEXT("HitStopTime : %f"), HitStopTime);
		if(!isParryed){
			GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime, false);
		}else{
			UE_LOG(LogTemp, Display, TEXT("ParryHitStop"));
			GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime/2, false);
		}
		//SetTimer(HitStopTimerHandle, this, AEnemy::EndHitStop(), HitStopTime, false);
	}
	
}

void AEnemy::EndHitStop()
{
	CustomTimeDilation = 1.0f;
	//Parry인지 아닌지 검사하는게 필요함 Note
	if(isParryed){
		CombatTarget->CustomTimeDilation = 5.0f;
	}else{
		CombatTarget->CustomTimeDilation = 1.0f;
	}
}

void AEnemy::PawnSeen(APawn * SeenPawn)
{
	const bool bShouldChaseTarget = 
		EnemyState != EEnemyState::EES_Dead &&			//죽지 않고
		EnemyState != EEnemyState::EES_Chasing &&		//이미 따라오는 상태가 아니며
		EnemyState < EEnemyState::EES_Attacking &&		//Attack하는 상태보단 심각하지 않다면 -> 현재까지는 Partroling 상태라면
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&	//그리고 본 캐릭터가 EngageableTarget이면 실행시킨다.
		!SeenPawn->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		EnemyMove->MoveToTarget(SeenPawn);
		ChaseTarget();
	}
}

bool AEnemy::CanAttack()
{
    return IsInSideMotionWarpAttackRadius() &&
	!IsAttacking() && 
	!IsDead() &&
	!IsEngage() &&
	!IsGetHitting();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if(HealthBarWidget){
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(EnemyCombat->AttackMin, EnemyCombat->AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ParryCheck()
{
	AVikingCharacter* vikingCharacter = Cast<AVikingCharacter>(CombatTarget);
	if(vikingCharacter){
		bool isCanParry = vikingCharacter->IsCanParry();

		if(isCanParry){
			isParryed = true;
			//Parry 당한 Animation 재생
			if(ParryedMontage){
				//UE_LOG(LogTemp, Display, TEXT("Enemy Parry"));
				PlayAnimMontage(ParryedMontage, 1.0f, TEXT("Default"));
			}		
			//시간 느리게 만들기
			GetWorldSettings()->SetTimeDilation(0.2f);
			GetWorldTimerManager().SetTimer(ParryTimerHandle, this, &AEnemy::RestoreParryTimeDilation, 0.7f, false);
			
			//viking Parry Animation 재생
			vikingCharacter->PlayParryAnimationMontage();
			vikingCharacter->SetTimeDilation(5.0f);

		}else{
			//UE_LOG(LogTemp, Display, TEXT("Enemy -> Cant Parry"));
		}
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("ParryChracter is not Viking"));
	}
}

void AEnemy::RestoreParryTimeDilation()
{
	isParryed = false;
	GetWorldSettings()->SetTimeDilation(1.0f);
	if(CombatTarget){
		CombatTarget->CustomTimeDilation = 1.0f;
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Miss CombatTarget"));
	}
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if(!IsDead()){
		ShowHealthBar();
	}
	
	PlayHitSound(ImpactPoint);
	EnemyMove->StopPatrollingTimer();
	ClearAttackTimer();

	StopAutoAttackMontage();
	StopMotionWarpAttackMontage();

	SetWeaponCollision(ECollisionEnabled::NoCollision);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	//CameraShake
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UDamageCameraShake::StaticClass());
	StartHitStop(DamageAmount, CombatTarget);		//맞았을 때 잠깐 시간이 멈춘것처럼 된다.
	EnemyMove->StopPatrollingTimer();
	EnemyState = EEnemyState::EES_GetHitting;

    return DamageAmount;
}

void AEnemy::Destoryed()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

void AEnemy::HideHealthBar()
{
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}
}
void AEnemy::ShowHealthBar()
{
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::CheckCombatTarget()
{
	if(IsOutSideCombatRadius()){	//쫓는 범위 밖
		ClearAttackTimer();			//공격 텀을 없앤다
		LoseInterest();				//더 이상 쫓지 않는다.
		
		if(!IsDead()){				
			StartPatrolling();
		}
	}
	else if(IsOutSideAttackRadius() && !IsChasing())		//공격범위가 아니면서 쫓는 상태가 아닐때
	{
		ClearAttackTimer();
		if(!IsEngage()){		//공격중 멀어졌을 때 따라가지 않게 하기 위함.
			ChaseTarget();
		}
	}else if(IsInSideAutoAttackRadius()){
		if(CanAttack()){		//범위 안에 있으면서 공격이 가능한 경우 공격 실시
			ChaseTarget();
			StartAttackTimer();
		}
	}
	else if(IsInSideMotionWarpAttackRadius())
	{	
		if(CanAttack()){		//범위 안에 있으면서 공격이 가능한 경우 공격 실시
			ChaseTarget();
			StartAttackTimer();
		}
	}
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;
	HideHealthBar();
}
void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = EnemyMove->GetPatrolingSpeed();
	EnemyMove->MoveToTarget(EnemyMove->GetPatrolTarget());
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = EnemyMove->GetChaseSpeed();
	
	EnemyMove->MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutSideCombatRadius()
{
    return EnemyMove->InTargetRange(CombatTarget, EnemyMove->GetCombatRadius()) == false;
}
bool AEnemy::IsOutSideAttackRadius()
{
	return EnemyMove->InTargetRange(CombatTarget, MotionWarpAttackRadius) == false;
}

bool AEnemy::IsInSideAutoAttackRadius()
{
    return EnemyMove->InTargetRange(CombatTarget, AutoAttackRadius);
}

bool AEnemy::IsInSideMotionWarpAttackRadius()
{
    return EnemyMove->InTargetRange(CombatTarget, MotionWarpAttackRadius);
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsGetHitting()
{
    return EnemyState == EEnemyState::EES_GetHitting;
}
bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}
bool AEnemy::IsEngage()
{
    return EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::SetHitting()
{
	EnemyState = EEnemyState::EES_GetHitting;
}

void AEnemy::GetHittingEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::ParryStunEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::SpawnHealItem()
{
	UWorld* World = GetWorld();
	if(World && HealthClass){
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
		World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	}
}