// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEAttack.h"
#include "EnemyFollowingAOEAttack.generated.h"

class AEnemy; 
class ABaseEnemyAIController;

UCLASS()
class SLASH_API AEnemyFollowingAOEAttack : public AEnemyAOEAttack
{
	GENERATED_BODY()

protected:
	AEnemyFollowingAOEAttack();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditDefaultsOnly)
	float FollowingSpeed = 100.f;

	AEnemy* OwnerEnemy;
	ABaseEnemyAIController* BaseEnemyAIController;
	AActor* AttackTarget;
};
