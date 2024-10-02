// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BaseEnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class SLASH_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABaseEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetEnemyState(EEnemyState State);

private:
	AEnemy* Enemy;
	EEnemyState EnemyState = EEnemyState::EES_Passive;

	virtual void CanSenceActor
};
