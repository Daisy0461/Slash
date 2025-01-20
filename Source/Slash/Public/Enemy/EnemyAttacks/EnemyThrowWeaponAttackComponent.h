// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyThrowWeaponAttackComponent.generated.h"

class AEnemy;
class USceneComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyThrowWeaponAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyThrowWeaponAttackComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayThrowWeaponAttackMontage();
	UFUNCTION()
	void StopThrowWeaponAttackMontage();
	UFUNCTION(BlueprintCallable)
	void SpawnThrowWeapon();

	AEnemy* OwnerEnemy;
	UPROPERTY(EditAnywhere)
	USceneComponent* ThrowPosition;
	UPROPERTY(EditDefaultsOnly, Category = "ThrowWeaponAttack")
	TSubclassOf<AActor> ThrowWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "ThrowWeaponAttack")
	UAnimMontage* ThrowWeaponAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "ThrowWeaponAttack")
	TArray<FName> ThrowWeaponAttackMontageSection;		//Section 추가 안하면 재생안함. -> 0개일 경우 Warning추가.
		
};
