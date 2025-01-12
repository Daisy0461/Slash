// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UEnemyAOEAttackComponent::UEnemyAOEAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	OwnerEnemy = Cast<AEnemy>(GetOwner());
}


// Called when the game starts
void UEnemyAOEAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!OwnerEnemy){
		AActor* Owner = GetOwner();
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr : %s (%s)"), *Owner->GetName(), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UEnemyAOEAttackComponent::EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType)
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	switch (AOEAttackType)
    {
    case EEnemyAOEAttackEnum::EEAA_SpinningAttack:
        PlaySpinningAttackMontage();
        break;
    case EEnemyAOEAttackEnum::EEAA_GroundAttack:
        PlayGroundAttackMontage();
        break;
    case EEnemyAOEAttackEnum::EEAA_MagicAreaAttack:
        PlayMagicAreaAttackMontage();
        break;
    case EEnemyAOEAttackEnum::EEAA_HealingArea:
        PlayHealingAreaMontage();
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid Attack Type"));
        break;
    }
}

void UEnemyAOEAttackComponent::PlaySpinningAttackMontage()
{
	if(OwnerEnemy->GetEnemyAnimInstance() && SpinningAttackMontage){
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(SpinningAttackMontage);
	}
}

void UEnemyAOEAttackComponent::PlayGroundAttackMontage()
{
	if(OwnerEnemy->GetEnemyAnimInstance() && GroundAttackMontage){
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(GroundAttackMontage);
	}
}

void UEnemyAOEAttackComponent::PlayMagicAreaAttackMontage()
{
	if(OwnerEnemy->GetEnemyAnimInstance() && MagicAreaAttackMontage){
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(MagicAreaAttackMontage);
	}
}

void UEnemyAOEAttackComponent::PlayHealingAreaMontage()
{
	if(OwnerEnemy->GetEnemyAnimInstance() && HealingAreaMontage){
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(HealingAreaMontage);
	}
}
