#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//Attribute는 삭제 예정 - HealthWidget이 있음.
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "TimerManager.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Animation/AnimInstance.h"
#include "HUD/HealthBarComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Health.h"
#include "Item/Weapons/Weapon.h"
#include "UObject/Class.h"
#include "Perception/AISense_Damage.h"

#include "DrawDebugHelpers.h"

AEnemy::AEnemy() 
{ 
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Enemy"));

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Components 추가
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetGenerateOverlapEvents(false);

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard Component"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CombatTarget = nullptr;
	//Tick비활성
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//HealthBarWiget 최초에 숨기기
	SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Idle);
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}

	if(HeadShotCurve){
		FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("HeadShotReaction"));

        HeadShotTimeline.AddInterpFloat(HeadShotCurve, TimelineProgress);
        HeadShotTimeline.SetLooping(false); // 반복 여부 설정

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("HeadShotReactionEnd"));
		HeadShotTimeline.SetTimelineFinishedFunc(TimelineFinished);
	}

	BaseEnemyAIController = Cast<ABaseEnemyAIController>(GetController());
	if(!BaseEnemyAIController){
		UE_LOG(LogTemp, Warning, TEXT("Base Enemy AI Controller is Null"));
	}
	
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HeadShotBlendValue > 0.0f){
		//UE_LOG(LogTemp, Display, TEXT("HeadShotBlendValue In Tick : %f"), HeadShotBlendValue);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("head"), HeadShotBlendValue);	
	}
	//if안에 두면 원상복구가 안됌 -> Finish가 안불림.
	HeadShotTimeline.TickTimeline(DeltaTime);
}

EEnemyState AEnemy::GetEnemyState()
{
	return EnemyState;
}

void AEnemy::Die()
{
	Super::Die();
	OnEnemyDeath.Broadcast();

	if(BaseEnemyAIController){
		BaseEnemyAIController->SetEnemyStateAsDead();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Base Enemy AI Controller is Null"));
	}
	//Animation 때문에 바꿔야함.
	EnemyState = EEnemyState::EES_Dead;
	
	//죽은 후 Collision 없애기 -> Mesh도 없애야함.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableCapsuleCollision();
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	//SetWeaponCollision(ECollisionEnabled::NoCollision);
	HideHealthBar();
	//Destoryed();
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(DestoryTime);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//Note: 랜덤적으로 Spawn하게 하면 좋을 듯 
	SpawnHealItem();
}

UBehaviorTree* AEnemy::GetBehaviorTree()
{
	if(!BehaviorTree){
		UE_LOG(LogTemp, Display, TEXT("Behavior Tree is None"));
	}

	return BehaviorTree;
}

UBlackboardComponent* AEnemy::GetBlackboardComponent() const 
{
	if(!BlackboardComponent){
		UE_LOG(LogTemp, Display, TEXT("BlackboardComponent is None"));
	}

	return BlackboardComponent;
}

float AEnemy::GetAttackRadius() const
{
	return AttackRadius;
}

float AEnemy::GetDefendRadius() const
{
	return DefendRadius;
}

void AEnemy::AttackByAI()
{
    Super::Attack(); 		//Play AutoAttack Montage

	if(AutoAttackMontage){
		//UE_LOG(LogTemp, Display, TEXT("In AutoAttack"));
		//섹션 이름을 꼭 더해줘야함.
		PlayAutoAttackMontage();
	}

}

void AEnemy::SetAIAttackDelegate(const FAIEnemyAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AEnemy::AttackEnd()
{
	//UE_LOG(LogTemp, Display, TEXT("AttackEnd"));
	OnAttackFinished.ExecuteIfBound();
}

void AEnemy::SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed)
{
    CurrentMovementSpeed = NewSpeed;

    // Enum에 따른 실제 이동 속도 설정
    switch (NewSpeed)
    {
        case EEnemyMovementSpeed::EEMS_Idle:
            GetCharacterMovement()->MaxWalkSpeed = 0.0f; 
            break;

        case EEnemyMovementSpeed::EEMS_Walk:
            GetCharacterMovement()->MaxWalkSpeed = 200.0f;  
            break;

        case EEnemyMovementSpeed::EEMS_Jogging:
            GetCharacterMovement()->MaxWalkSpeed = 400.0f; 
            break;

        case EEnemyMovementSpeed::EEMS_Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = 600.0f; 
            break;

        default:
            break;
    }
}

EEnemyMovementSpeed AEnemy::GetMovementSpeedEnum() const
{
    return CurrentMovementSpeed;
}

void AEnemy::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
}

bool AEnemy::CanAttack()
{
    return
	!IsAttacking() && 
	!IsDead() &&
	!IsStrafing() &&
	!IsGetHitting();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if(HealthBarWidget){
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}else{
		UE_LOG(LogTemp, Warning, TEXT("can't find HealthBar Widget"));
	}
}

AActor* AEnemy::GetPatrolRoute() const
{
	return PatrollSpline;
}

void AEnemy::Healing(float HealAmount)
{
	Super::Healing(HealAmount);
	if(HealthBarWidget){
		//UE_LOG(LogTemp, Display, TEXT("Healing"));
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		ShowHealthBar();
	}
}

void AEnemy::PlayStunMontage()
{
	if(ParryedMontage){
		FName parrySection = TEXT("Default");
		ChoosePlayMontageSection(ParryedMontage, parrySection);
	}
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if(Attributes->GetHealthPercent() > 0.f){
		ShowHealthBar();
	}
	
	PlayHitSound(ImpactPoint);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UVikingCameraShake::StaticClass());

	StopAutoAttackMontage();
	StopMovement();

	// if(Weapon){
	// 	SetWeaponCollision(Weapon, ECollisionEnabled::NoCollision);
	// }

	//Hit Sence를 위해서 하고 있음.
	//UE_LOG(LogTemp, Display, TEXT("Damage Sence"));
	if (Hitter)
    {
        UAISense_Damage::ReportDamageEvent(
            GetWorld(),
            this,       // 데미지를 입은 액터 (AI 캐릭터)
            Hitter,     // 공격자 (플레이어 등)
            10.0f,      // 데미지 양 (필요에 따라 실제 데미지 값으로 대체)
            ImpactPoint,
            ImpactPoint - GetActorLocation()  // 데미지가 어디서 왔는지 확인가능.
        );
    }
}

void AEnemy::GetHeadShot(FVector ImpactPoint)
{
	UE_LOG(LogTemp, Display, TEXT("Enemy Head Shot"));

	if(HeadShotHitSound){
        UGameplayStatics::PlaySoundAtLocation(this, HeadShotHitSound, GetActorLocation(), 1.f, 1.f);
    }

	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("head"), true);
	HeadShotTimeline.PlayFromStart();

	GetWorld()->GetTimerManager().SetTimer(HeadShotImpulseDelayTimerHandle, [this, ImpactPoint]()
	{
		HeadShotAddImpulse(ImpactPoint); 
	}, 0.48f, false);
}

void AEnemy::HeadShotReaction(float Value)
{
	HeadShotBlendValue = Value;
}

void AEnemy::HeadShotAddImpulse(FVector ImpactPoint)
{
	FVector NormalizedImpact = ImpactPoint.GetSafeNormal();
	GetMesh()->AddImpulse(NormalizedImpact, TEXT("head"));
}

void AEnemy::HeadShotReactionEnd()
{
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("head"), 0);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	HandleDamage(DamageAmount);
	//UE_LOG(LogTemp, Display, TEXT("Take Damage"));
	
    return DamageAmount;
}


void AEnemy::HideHealthBar()
{
	if(HealthBarWidget){
		//UE_LOG(LogTemp, Display, TEXT("In Hide"));
		HealthBarWidget->SetVisibility(false);
	}
}
void AEnemy::ShowHealthBar()
{
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(true);
	}
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsGetHitting()
{
    return EnemyState == EEnemyState::EES_Hitting;
}
bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}
bool AEnemy::IsStrafing()
{
    return EnemyState == EEnemyState::EES_Strafing;
}

bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::SetHitting()
{
	EnemyState = EEnemyState::EES_Hitting;
	UE_LOG(LogTemp, Display, TEXT("Set Hitting In Enemy"));
}

void AEnemy::GetHittingEnd()
{
	EnemyState = EEnemyState::EES_Strafing;
	UE_LOG(LogTemp, Display, TEXT("Get Hitting In Enemy End"));
	//CheckCombatTarget();
}

void AEnemy::BreakSkeletalBone(FVector ImpactPoint, FVector ImpactNormal, FName BreakBoneName)
{
	// if(EnemyState == EEnemyState::Dead) return;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if(MeshComp){
		MeshComp->BreakConstraint(ImpactPoint, ImpactNormal * -100, BreakBoneName);
		UE_LOG(LogTemp, Warning, TEXT("Broke Constraint on Bone: %s at Location: %s"), *BreakBoneName.ToString(), *ImpactPoint.ToString());
	}else{
		UE_LOG(LogTemp, Warning, TEXT("In Break Skeletal Mesh, Mesh is nullptr"));
	}
}

void AEnemy::SpawnHealItem()
{
	UWorld* World = GetWorld();
	if(World && HealthClass){
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
		World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	}
}