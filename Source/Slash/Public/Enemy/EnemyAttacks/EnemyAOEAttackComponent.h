// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackEnum.h"
#include "Components/TimelineComponent.h"
#include "EnemyAOEAttackComponent.generated.h"

class AEnemy;
class UAnimMontage;
class AEnemyAOEAttack;
class AEnemyAreaHeal;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyAOEAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyAOEAttackComponent();

	UFUNCTION(BlueprintCallable)
	virtual void EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType);

	//Spinning Attack
	UFUNCTION(BlueprintCallable)
	void SpinMeshTimelineControll(bool bIsStart);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AEnemy* OwnerEnemy;

	//Spinning Attack
	UFUNCTION()
	void PlaySpinningAttackMontage();
	UFUNCTION()
	void SpinMesh(float Value);
	UFUNCTION(BlueprintCallable) 
	void SpinAOESpawn();
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	UAnimMontage* SpinningAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	TArray<FName> SpinningAttackMontageSections;
	FTimeline SpinMeshTimeline;
	UPROPERTY(EditAnywhere, Category = "Spinning Attack")
	TSubclassOf<AEnemyAOEAttack> SpinningAOEAttack;
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	UCurveFloat* SpinCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Spinning Attack")
	float SpinValue = 15.f;
	
	//Ground Attack
	UFUNCTION()
	void PlayGroundAttackMontage();
	UFUNCTION(BlueprintCallable)
	virtual void SpawnGroundAOE();
	UPROPERTY(EditDefaultsOnly, Category = "Ground Attack")
	UAnimMontage* GroundAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Ground Attack")
	TArray<FName> GroundAttackMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Ground Attack")
	TSubclassOf<AEnemyAOEAttack> GroundAOEClass;
	float GroundForwardOffset = 100.f;

	//MagicArea Attack
	UFUNCTION()
	void PlayMagicAreaAttackMontage();
	UFUNCTION(BlueprintCallable)
	void SpawnMagicAreaAOE();
	UPROPERTY(EditDefaultsOnly, Category = "MagicArea Attack")
	UAnimMontage* MagicAreaAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "MagicArea Attack")
	TArray<FName> MagicAreaAttackMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "MagicArea Attack")
	TSubclassOf<AEnemyAOEAttack> MagicAreaAOEClass;

	//Healing
	UFUNCTION()
	void PlayHealingAreaMontage();
	void SpawnHealingArea();
	void OnHealingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void DestroyHealingArea();
	UPROPERTY(EditDefaultsOnly, Category = "HealingArea")
	UAnimMontage* HealingAreaMontage;
	UPROPERTY(EditDefaultsOnly, Category = "HealingArea")
	TArray<FName> HealingAreaMontageSections;
	UPROPERTY(EditDefaultsOnly, Category = "Healing")
	TSubclassOf<AEnemyAreaHeal> HealingAreaClass;
	AEnemyAreaHeal* HealingArea;

};
