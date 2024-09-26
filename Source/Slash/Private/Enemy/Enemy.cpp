#include "Enemy/Enemy.h"
#include "Enemy/EnemyMoveComponent.h"
#include "Enemy/EnemyCombat.h"
//VikingCharacter는 Interface만들어서 그걸로 대체하자..
#include "Character/VikingCharacter.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "HUD/HealthBarComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Item/Health.h"
#include "Item/Weapons/Weapon.h"
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
	//EnemyMove = CreateDefaultSubobject<UEnemyMoveComponent>(TEXT("EnemyMoveComponent"));
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 45.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CombatTarget = nullptr;
	AttackMoveMaxDistance = 0.f; 
	AttackingMoveSpeed = 2.0f;

	//Tick비활성
	PrimaryActorTick.bCanEverTick = false;
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
	
	// BT_Test
	//EnemyState = EEnemyState::EES_Dead;
	//죽은 후 Collision 없애기
	DisableCapsuleCollision();
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


void AEnemy::StartHitStop(float DamageAmount)
{
	// CustomTimeDilation = 0.0f;
	// //PlayerActor->CustomTimeDilation = 0.0f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f);
	UE_LOG(LogTemp, Display, TEXT("HitStop"));
	float HitStopTime = DamageAmount * HitStopModifier;

	if(GetWorld()){
		if(isParryed){
			GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime/5, false);
		}else{
			GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AEnemy::EndHitStop, HitStopTime, false);
		}
		//SetTimer(HitStopTimerHandle, this, AEnemy::EndHitStop(), HitStopTime, false);
	}
}

void AEnemy::EndHitStop()
{
	CustomTimeDilation = 1.0f;
	if(isParryed && CombatTarget){
		CombatTarget->CustomTimeDilation = 5.0f;
	}else{
		CombatTarget->CustomTimeDilation = 1.0f;
	}
}

bool AEnemy::CanAttack()
{
    return
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

void AEnemy::ParryCheck()
{
	if(CombatTarget)
	{
		AVikingCharacter* viking = Cast<AVikingCharacter>(CombatTarget);
		if(viking){
			bool parryed = viking->IsCanParry();

			if(parryed){
				// BT_Test
				//EnemyState = EEnemyState::EES_Parryed;
				isParryed = true;
				FName parrySection = TEXT("Default");
				ChoosePlayMontageSection(ParryedMontage, parrySection);

				//시간 느리게 viking 은 빠르게
				GetWorldSettings()->SetTimeDilation(0.2f);
				viking->SetCustiomTimeDilation(5.f);

				GetWorldTimerManager().SetTimer(ParryTimer, this, &AEnemy::ParryStunEnd, 0.5f);
			}
		}
	}
}

void AEnemy::ParryStunEnd()
{
	//UE_LOG(LogTemp, Display, TEXT("Parry Stun End"));
	GetWorldSettings()->SetTimeDilation(1.f);
	AVikingCharacter* viking = Cast<AVikingCharacter>(CombatTarget);
	viking->SetCustiomTimeDilation(1.f);
	isParryed = false;
}


void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if(!IsDead()){
		//UE_LOG(LogTemp, Display, TEXT("In Get Hit Implementation"));
		ShowHealthBar();
	}

	//UE_LOG(LogTemp, Display, TEXT("Impact Point %f, %f, %f"), ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z);
	SpawnHitParticle(ImpactPoint);
	PlayHitSound(ImpactPoint);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UVikingCameraShake::StaticClass());

	StopAutoAttackMontage();

	SetWeaponCollision(Weapon, ECollisionEnabled::NoCollision);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	//UE_LOG(LogTemp, Display, TEXT("Take Damage"));
	HandleDamage(DamageAmount);
	//CombatTarget = EventInstigator->GetPawn();
	if(CombatTarget){
		StartHitStop(DamageAmount);		//맞았을 때 잠깐 시간이 멈춘것처럼 된다.
	}else{
		//Hit Error Cause Here
		//UE_LOG(LogTemp, Display, TEXT("Take Damage Can't find CombatTarget"));
	}

	//EnemyMove->StopPatrollingTimer();
	// BT_Test
	//EnemyState = EEnemyState::EES_GetHitting;

    return DamageAmount;
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
	}else{
		UE_LOG(LogTemp, Display, TEXT("Can't find Health Bar"));
	}
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