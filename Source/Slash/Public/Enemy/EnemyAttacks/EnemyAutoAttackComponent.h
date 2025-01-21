// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAutoAttackComponent.generated.h"

class UAnimMontage;
class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyAutoAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyAutoAttackComponent();
	UFUNCTION()
	void PlayRandomAutoAttackMontage(bool bIsRandom, int32 SelectMontageNumber);
	UFUNCTION()
	void StopAutoAttackMontage();

	// Called when the game starts
	virtual void BeginPlay() override;

	AEnemy* OwnerEnemy;
	UPROPERTY(EditDefaultsOnly, Category = "AutoAttack")
	UAnimMontage* AutoAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "AutoAttack")
	TArray<FName> AutoAttackMontageSection;		//Section 추가 안하면 재생안함. -> 0개일 경우 Warning추가.

	//Montage 순서와 Section 순서가 동일해야 정상적인 작동을 함.
	UPROPERTY(EditDefaultsOnly, Category = "AutoAttack")
	TArray<UAnimMontage*> SelectAutoAttackMontages;
	UPROPERTY(EditDefaultsOnly, Category = "AutoAttack")
	TArray<FName> SelectAutoAttackMontageSections;
};