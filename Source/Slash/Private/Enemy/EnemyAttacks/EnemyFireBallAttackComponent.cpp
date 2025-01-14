// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"

// Sets default values for this component's properties
UEnemyFireBallAttackComponent::UEnemyFireBallAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFireBallAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyFireBallAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UEnemyFireBallAttackComponent::EnemyFireBallAttack(EEnemyFireBallEnum FireBallType)
{
	switch (FireBallType)
	{
	case EEnemyFireBallEnum::EFBE_BasicFireBall:
		//PlayFireBallMontage();
		break;
	case EEnemyFireBallEnum::EFBE_BarrageFireBall:
		//PlayFireBallBarrageMontage();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid FireBall Type"));
		break;
	}
}

