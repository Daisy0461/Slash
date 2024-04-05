#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Enemy/EnemyCombat.h"
#include "Character/VikingCharacter.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Animation/AnimInstance.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "Item/Health.h"
#include "UObject/Class.h"

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
			DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
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
		PlayAutoAttackMontage();
	}else if(MotionWarpAttackMontage && IsInSideMotionWarpAttackRadius()){
		PlayMotionWarpAttackMontage();
	}
	EnemyState = EEnemyState::EES_Engaged;
}

void AEnemy::SpawnFireBall()
{
	if(!FireBallActor) return;

	if(GetWorld()){
		auto FireBall = GetWorld()->SpawnActor<AActor>(
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
		GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime, false);
		//SetTimer(HitStopTimerHandle, this, AEnemy::EndHitStop(), HitStopTime, false);
	}
}

void AEnemy::EndHitStop()
{
	CustomTimeDilation = 1.0f;
	CombatTarget->CustomTimeDilation = 1.0f;
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
	if(CombatTarget)
	{
		AVikingCharacter* viking = Cast<AVikingCharacter>(CombatTarget);
		if(viking){
			bool parryed = viking->IsCanParry();

			if(parryed){
				EnemyState = EEnemyState::EES_Parryed;
				FName parrySection = TEXT("Default");
				ChoosePlayMontageSection(ParryedMontage, parrySection);

				//시간 느리게 viking 은 빠르게
				GetWorldSettings()->SetTimeDilation(0.2f);
				viking->SetCustiomTimeDilation(5.f);
			}
		}
	}
}

void AEnemy::ParryStunEnd()
{
	GetWorldSettings()->SetTimeDilation(1.f);
	AVikingCharacter* viking = Cast<AVikingCharacter>(CombatTarget);
	viking->SetCustiomTimeDilation(1.f);
}


void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if(!IsDead()){
		ShowHealthBar();
	}

	SpawnHitParticle(ImpactPoint);
	PlayHitSound(ImpactPoint);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UVikingCameraShake::StaticClass());


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
			StartParoling();
		}
	}
	else if(IsOutSideAttackRadius() && !IsChasing())		//공격범위가 아니면서 쫓는 상태가 아닐때
	{
		ClearAttackTimer();
		if(!IsEngage()){		//공격중 멀어졌을 때 따라가지 않게 하기 위함.
			ChaseTarget();
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
void AEnemy::StartParoling()
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

void AEnemy::SpawnHealItem()
{
	UWorld* World = GetWorld();
	if(World && HealthClass){
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
		World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	}
}