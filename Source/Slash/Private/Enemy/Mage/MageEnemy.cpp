#include "Enemy/Mage/MageEnemy.h"
#include "Enemy/Mage/MageEnemyAIController.h"
#include "Enemy/EnemyAreaHeal.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyTeleportComponent.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"       //속도 변환용 헤더파일
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AMageEnemy::AMageEnemy()
{
    AttackRadius = 2000.f;
    DefendRadius = 3000.f;

    EnemyAOEAttackComponent = CreateDefaultSubobject<UEnemyAOEAttackComponent>(TEXT("EnemyAOEAttackComponent"));
    EnemyFireBallComponent = CreateDefaultSubobject<UEnemyFireBallAttackComponent>(TEXT("EnemyFireBallAttackComponent"));
    EnemyTeleportComponent = CreateDefaultSubobject<UEnemyTeleportComponent>(TEXT("EnemyTeleportComponent"));
    EnemyTeleportComponent->bAutoActivate = false;

    //GetCharacterMovement()->MaxFlySpeed = TeleportSpeed;
    PrimaryActorTick.bCanEverTick = true;
}

void AMageEnemy::BeginPlay()
{
    Super::BeginPlay();
}

void AMageEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMageEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
}