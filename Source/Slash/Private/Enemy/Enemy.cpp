#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Enemy/EnemyCombat.h"
#include "Components/AttributeComponent.h"
#include "Animation/AnimInstance.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"


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
	PawnSensing->SightRadius = 45.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UWorld* World = GetWorld();
	if(World && WeaponClass){
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		AShield* DefaultShield = World->SpawnActor<AShield>(ShieldClass);
		if(DefaultWeapon){
			DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
			EquippedWeapon = DefaultWeapon;
		}
		if(DefaultShield){
			DefaultShield->Equip(GetMesh(), FName("LeftHandSocket"));
		}
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsDead()) return;

	if(EnemyState > EEnemyState::EES_Patrolling){
		CheckCombatTarget();
	}else{
		EnemyMove->CheckPatrolTarget();
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	//HealthBarWiget 최초에 숨기기
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}

	if(PawnSensing){
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}
}

void AEnemy::Die()
{
	EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	//죽은 후 Collision 없애기
	DisableCapsuleCollision();
	HideHealthBar();
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(DestoryTime);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if(Pose < EDeathPose::EDP_Max){
		DeathPose = Pose;
	}
	return Selection;
}

void AEnemy::Attack()
{
	EnemyState = EEnemyState::EES_Engaged;
	Super::Attack();
	PlayAttackMontage();
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
		SeenPawn->ActorHasTag(FName("MainCharacter"));	//그리고 본 캐릭터가 MainCharacter이면 실행시킨다.

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		EnemyMove->StopPatrollingTimer();
		ChaseTarget();
	}
}

bool AEnemy::CanAttack()
{
    return IsInSideAttackRadius() && 
	!IsAttacking() && 
	!IsDead() &&
	!IsEngage();
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

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint)
{
	ShowHealthBar();

	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);

	if(IsAlive()){
		DirectionalHitReact(ImpactPoint);
	}else{
		Die();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	HandleDamage(DamageAmount);
	StartHitStop(DamageAmount, CombatTarget);		//맞았을 때 잠깐 시간이 멈춘것처럼 된다.
	CombatTarget = EventInstigator->GetPawn();
	ChaseTarget();
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
	if(IsOutSideCombatRadius()){	
		ClearAttackTimer();
		LoseInterest();
		
		if(IsEngage()){
			StartParoling();
		}
	}
	else if(IsOutSideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if(!IsEngage()){
			ChaseTarget();
		}
	}
	else if(CanAttack())
	{
		StartAttackTimer();
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
	EnemyMove->MoveToTarget(CombatTarget);
	GetCharacterMovement()->MaxWalkSpeed = EnemyMove->GetChaseSpeed();
}

bool AEnemy::IsOutSideCombatRadius()
{
    return EnemyMove->InTargetRange(CombatTarget, EnemyMove->GetCombatRadius()) == false;
}
bool AEnemy::IsOutSideAttackRadius()
{
	return EnemyMove->InTargetRange(CombatTarget, AttackRadius) == false;
}
bool AEnemy::IsInSideAttackRadius()
{
    return EnemyMove->InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}
bool AEnemy::IsEngage()
{
    return EnemyState == EEnemyState::EES_Engaged;
}
bool AEnemy::IsAlive()
{
    return Attributes && Attributes->IsAlive();
}
bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}
