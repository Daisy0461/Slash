// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "EnemyFireBallAttackComponent.generated.h"

class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyFireBallAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFireBallAttackComponent();

	UFUNCTION(BlueprintCallable)
	virtual void EnemyFireBallAttack(EEnemyFireBallEnum FireBallType);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	AEnemy* OwnerEnemy;
	UPROPERTY(EditAnywhere)
    USceneComponent* FirePosition;

	//BasicFireBall
	void PlayBasicFireBallMontage();
	UFUNCTION(BlueprintCallable)
	void SpawnBasicFireBall();
	UPROPERTY(EditDefaultsOnly, Category = "Basic FireBall")
	TSubclassOf<AActor> BasicFireBall;
	UPROPERTY(EditDefaultsOnly, Category = "Basic FireBall")
	UAnimMontage* BasicFireBallMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Basic FireBall")
	TArray<FName> BasicFireBallMontageSections;

	//Barrage
	void PlayFireBallBarrageMontage();
	UFUNCTION(BlueprintCallable)
	void SpawnBarrageFireBall();
	UPROPERTY(EditDefaultsOnly, Category = "Barrage FireBall")
	TSubclassOf<AActor> BarrageFireBall;
	UPROPERTY(EditDefaultsOnly, Category = "Barrage FireBall")
	UAnimMontage* BarrageFireBallMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Barrage FireBall")
	TArray<FName> BarrageFireBallMontageSections;

		
};
