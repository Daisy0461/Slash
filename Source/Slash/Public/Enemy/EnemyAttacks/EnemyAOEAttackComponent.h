// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackEnum.h"
#include "EnemyAOEAttackComponent.generated.h"

class AEnemy;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyAOEAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyAOEAttackComponent();

	UFUNCTION(BlueprintCallable)
	virtual void EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AEnemy* OwnerEnemy;

	UFUNCTION()
	void PlaySpinningAttackMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	UAnimMontage* SpinningAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	TArray<FName> SpinningAttackMontageSections;
	
	UFUNCTION()
	void PlayGroundAttackMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Ground Attack")
	UAnimMontage* GroundAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Ground Attack")
	TArray<FName> GroundAttackMontageSections;

	UFUNCTION()
	void PlayMagicAreaAttackMontage();
	UPROPERTY(EditDefaultsOnly, Category = "MagicArea Attack")
	UAnimMontage* MagicAreaAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "MagicArea Attack")
	TArray<FName> MagicAreaAttackMontageSections;

	UFUNCTION()
	void PlayHealingAreaMontage();
	UPROPERTY(EditDefaultsOnly, Category = "HealingArea")
	UAnimMontage* HealingAreaMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HealingArea")
	TArray<FName> HealingAreaMontageSections;

};
