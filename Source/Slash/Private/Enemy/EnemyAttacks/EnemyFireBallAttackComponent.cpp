// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UEnemyFireBallAttackComponent::UEnemyFireBallAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	OwnerEnemy = Cast<AEnemy>(GetOwner());

	if(OwnerEnemy){
		FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
		FirePosition->SetupAttachment(OwnerEnemy->GetRootComponent());
	}
}


// Called when the game starts
void UEnemyFireBallAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UEnemyFireBallAttackComponent::EnemyFireBallAttack(EEnemyFireBallEnum FireBallType)
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr; %s (%s)"), *OwnerEnemy->GetName(), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	switch (FireBallType)
	{
	case EEnemyFireBallEnum::EFBE_BasicFireBall:
		//PlayBasicFireBallMontage();
		break;
	case EEnemyFireBallEnum::EFBE_BarrageFireBall:
		//PlayFireBallBarrageMontage();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid FireBall Type"));
		break;
	}
}

void UEnemyFireBallAttackComponent::PlayBasicFireBallMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!BasicFireBallMontage){
		UE_LOG(LogTemp, Warning, TEXT("BasicFireBallMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!BasicFireBallMontageSections.Num()){
		UE_LOG(LogTemp, Warning, TEXT("BasicFireBallMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, BasicFireBallMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(BasicFireBallMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(BasicFireBallMontageSections[RandomIndex], BasicFireBallMontage);
}

void UEnemyFireBallAttackComponent::SpawnBasicFireBall()
{
	if(!FirePosition || !BasicFireBall || !OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("SpawnBasicFireBall Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	const FVector SpawnLocation = FirePosition->GetComponentLocation();

	AActor* SpawnedFireBall = OwnerEnemy->GetWorld()->SpawnActor<AActor>(BasicFireBall, SpawnLocation, OwnerEnemy->GetActorRotation());
	if(SpawnedFireBall){
            SpawnedFireBall->SetOwner(OwnerEnemy);
            SpawnedFireBall->SetInstigator(Cast<APawn>(OwnerEnemy));
        }else{
            UE_LOG(LogTemp, Warning, TEXT("FireBall Spawn Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void UEnemyFireBallAttackComponent::PlayFireBallBarrageMontage()
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	
}
