#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Enemy/EnemyCombat.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//Attribute는 삭제 예정 - HealthWidget이 있음.
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
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

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Components 추가
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

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
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

EEnemyState AEnemy::GetEnemyState()
{
	return EnemyState;
}

void AEnemy::Die()
{
	Super::Die();
	OnEnemyDeath.Broadcast();
	
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
	if(Weapon){ 
		Weapon->OverlappedActorClear();
	}
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


float AEnemy::CheckTargetDistance()
{
	float Distance = 180.f;
	if(CombatTarget){
		Distance = GetDistanceTo(CombatTarget);
		//UE_LOG(LogTemp, Display, TEXT("Distance : %f"), Distance);

		if(Distance < 120.f){
			Distance = 120.f;
		}else if(Distance > 180.f){
			Distance = 180.f;
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("Check Target Distance : %f"), Distance);

	return Distance;
}

void AEnemy::AttackRotate()
{
	Super::AttackRotate();

	if(CombatTarget){
		//UE_LOG(LogTemp, Display, TEXT("In Attack Rotate"));
		const FVector CuurentActorLocation = GetActorLocation();
		const FVector LockedOnActorLocation = CombatTarget->GetActorLocation();
		FVector Direction = LockedOnActorLocation - CuurentActorLocation;
		Direction.Z = 0.0f;		//Z축 회전 무시

		FRotator ActorRotate = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetActorRotation(ActorRotate);
	}
}

AWeapon* AEnemy::GetWeapon(){
	Super::GetWeapon();
	return Weapon;
}

void AEnemy::SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	if(CollisionWeapon && CollisionWeapon->GetParryBox())
	{	
		CollisionWeapon->IgnoreActors.Empty();
		CollisionWeapon->IgnoreActors.Add(GetOwner());

		CollisionWeapon->GetParryBox()->SetCollisionEnabled(CollisionType);
	}else if(!CollisionWeapon){
		UE_LOG(LogTemp, Display, TEXT("Can't Find Weapon in ParryBoxCollision"));
	}
}

void AEnemy::PlayStunMontage()
{
	FName parrySection = TEXT("Default");
	ChoosePlayMontageSection(ParryedMontage, parrySection);
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	//Weapon에서 HitInterface라는 함수에서 이 함수를 실행시킨다.
	//Enemy에서 Hitter는 Viking이 된다.
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	//BaseEnemyAIController에서 Broadcast 받음.
	//OnEnemyHit.Broadcast(); //DamageSence로 대체예정

	if(Attributes->GetHealthPercent() > 0.f){
		ShowHealthBar();
	}
	
	PlayHitSound(ImpactPoint);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UVikingCameraShake::StaticClass());

	StopAutoAttackMontage();
	StopMovement();

	if(Weapon){
		SetWeaponCollision(Weapon, ECollisionEnabled::NoCollision);
	}

	//Hit Sence를 위해서 하고 있음.
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
		//UE_LOG(LogTemp, Warning, TEXT("Find Health Bar"));
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
}

void AEnemy::GetHittingEnd()
{
	EnemyState = EEnemyState::EES_Strafing;
	//CheckCombatTarget();
}

void AEnemy::SpawnHealItem()
{
	UWorld* World = GetWorld();
	if(World && HealthClass){
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
		World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	}
}