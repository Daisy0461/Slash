#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
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
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	EnemyMove = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("EnemyMoveComponent"));
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

		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
		DefaultShield->Equip(GetMesh(), FName("LeftHandSocket"));
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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(EnemyState > EEnemyState::EES_Patrolling){
		CheckCombatTarget();
	}
}



void AEnemy::PawnSeen(APawn * SeenPawn)
{
	if(EnemyState == EEnemyState::EES_Chasing) return;

	if(SeenPawn->ActorHasTag(FName("MainCharacter"))){		//Cast로 찾는것 보다 Tag로 찾는게 더 낫다.
		EnemyMove->StopPatrollingTimer();
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		CombatTarget = SeenPawn;				//CombatTarget으로 설정을 해야 ChestCombatTarget으로 범위를 벗어났을 때 더 이상 오지 않는다.

		if(EnemyState != EEnemyState::EES_Attacking){
			EnemyState = EEnemyState::EES_Chasing;
			EnemyMove->MoveToTarget(CombatTarget);
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();
	PlayAttackMontage();
}

void AEnemy::PlayAttackMontage()
{
	Super::PlayAttackMontage();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage){
		AnimInstance -> Montage_Play(AttackMontage);
		FName SectionName = FName();
		const int32 Selection = FMath::RandRange(0, 2);
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		default:
			SectionName = FName("Attack1");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
			UE_LOG(LogTemp, Warning, TEXT("Attack"));
	}
}

void AEnemy::CheckCombatTarget()
{
	if(EnemyMove->InTargetRange(CombatTarget, EnemyMove->GetCombatRadius()) == false){	
		//CombatTarget과 거리가 멀어야 CombatTarget = nullptr;
		CombatTarget = nullptr;
		if(HealthBarWidget){
			HealthBarWidget->SetVisibility(false);
		}

		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = 130.f;
		EnemyMove->MoveToTarget(EnemyMove->GetPatrolTarget());
	}
	else if(EnemyMove->InTargetRange(CombatTarget, AttackRadius) == false && EnemyState!=EEnemyState::EES_Chasing)
	{
		//AttackRange에서 벗어났을 때 -> Attack을 하지 않고 Chase
		EnemyState = EEnemyState::EES_Chasing;
		EnemyMove->MoveToTarget(CombatTarget);
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
	}
	else if(EnemyMove->InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		//공격범위 안에 있을 때
		EnemyState = EEnemyState::EES_Attacking;
		//Attack Animation
		Attack();
	}
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint)
{
	//Play_Warrior_HitReact_Montage(FName("FromRight"));
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(true);
	}

	if(HitSound){		//Sound 재생
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if(HitParticles && GetWorld()){		//Particle 재생
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles, 
			ImpactPoint
		);
	}

	if(Attributes && Attributes->IsAlive()){		//살아있기 때문에 Hit React 재생
		//UE_LOG(LogTemp, Display, TEXT("Your message"));
		DirectionalHitReact(ImpactPoint);
	}

	if(Attributes && !Attributes->IsAlive()){		//죽을 때 실행
		Die();
	}
	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	if(Attributes && HealthBarWidget){
		Attributes->ReceiveDamage(DamageAmount);	
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	CombatTarget = EventInstigator->GetPawn();

	if(CombatTarget){
		StartHitStop(DamageAmount, CombatTarget);		//맞았을 때 잠깐 시간이 멈춘것처럼 된다.

		EnemyState = EEnemyState::EES_Chasing;
		EnemyMove->StopPatrollingTimer();
		GetCharacterMovement()->MaxWalkSpeed=400.f;
		EnemyMove->MoveToTarget(CombatTarget);	//나에게 데미지를 준 CombatTarget에게 이동함.
	}

    return DamageAmount;
}

void AEnemy::Die()
{
	//Die Montage Play
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DieMontage){
		AnimInstance->Montage_Play(DieMontage);
		AnimInstance->Montage_JumpToSection(SelectDieAnimation(), DieMontage);
	}

	//죽은 후 Collision 없애기
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthBarWidget->SetVisibility(false);
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(5.f);
	
}

void AEnemy::Destoryed()
{
	if(EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
}

FName AEnemy::SelectDieAnimation()
{
	int32 SelectNum = FMath::RandRange(0, 4);
	FName SectionName = FName("Death_1");
	switch (SelectNum)
	{
	case 1:
		SectionName = FName("Death_1");
		DeathPose = EDeathPose::EDP_Dead_1;
		break;
	case 2:
		SectionName = FName("Death_2");
		DeathPose = EDeathPose::EDP_Dead_2;
		break;
	case 3:
		SectionName = FName("Death_3");
		DeathPose = EDeathPose::EDP_Dead_3;
		break;
	case 4:
		SectionName = FName("Death_4");
		DeathPose = EDeathPose::EDP_Dead_4;
		break;
	case 5:
		SectionName = FName("Death_5");
		DeathPose = EDeathPose::EDP_Dead_5;
		break;
	default:
		break;
	}

    return SectionName;
}

void AEnemy::EndHitStop()
{
	CustomTimeDilation = 1.0f;
	CombatTarget->CustomTimeDilation = 1.0f;
}

void AEnemy::StartHitStop(float DamageAmount, AActor* PlayerActor)
{
	CustomTimeDilation = 0.0f;
	PlayerActor->CustomTimeDilation = 0.0f;
	float HitStopTime = DamageAmount * HitStopModifier;

	if(GetWorld()){
		UE_LOG(LogTemp, Display, TEXT("Start Hit Stop"));
		GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime, false);
		//SetTimer(HitStopTimerHandle, this, AEnemy::EndHitStop(), HitStopTime, false);
	}
}