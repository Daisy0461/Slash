// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"
#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UEnemyAutoAttackComponent::UEnemyAutoAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	OwnerEnemy = Cast<AEnemy>(GetOwner());
}

// Called when the game starts
void UEnemyAutoAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	if(!OwnerEnemy){
		AActor* Owner = GetOwner();
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr : %s (%s)"), *Owner->GetName(), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UEnemyAutoAttackComponent::PlayRandomAutoAttackMontage(bool bIsRandom, int32 SelectMontageNumber)
{
	if(AutoAttackMontageSection.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoAttackMontageSection is Empty (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!AutoAttackMontage || !OwnerEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("AutoAttackMontage or OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	if(bIsRandom){
		int32 SectionIndex = FMath::RandRange(0, AutoAttackMontageSection.Num() - 1);
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(AutoAttackMontage);
		OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(AutoAttackMontageSection[SectionIndex], AutoAttackMontage);
	}else{
		int32 SectionIndex = SelectMontageNumber;
		if(SectionIndex < 0 || SectionIndex >= SelectAutoAttackMontages.Num()){
			UE_LOG(LogTemp, Warning, TEXT("AutoAttack SectionNumber is not vaild (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}
		if (SelectAutoAttackMontages.Num() != SelectAutoAttackMontageSections.Num()) {
			UE_LOG(LogTemp, Error, TEXT("AutoAttackMontages and AutoAttackMontageSections size mismatch (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}
		OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(SelectAutoAttackMontages[SectionIndex]);
		OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(SelectAutoAttackMontageSections[SectionIndex], SelectAutoAttackMontages[SectionIndex]);
	}
}

void UEnemyAutoAttackComponent::StopAutoAttackMontage()
{
	if(!OwnerEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	UAnimInstance* AnimInstance = OwnerEnemy->GetEnemyAnimInstance();
	if(AnimInstance && AutoAttackMontage)
	{
		AnimInstance->Montage_Stop(0.45f, AutoAttackMontage);
	}
}
