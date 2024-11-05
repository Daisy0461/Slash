// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "WarriorEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AWarriorEnemyAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()
public:
	AWarriorEnemyAIController();
	virtual void SetEnemyStateAsAttacking(AActor* AttackTarget) override;
	virtual void SetEnemyStateAsHitting(AActor* AttackTarget) override;
	virtual void SetEnemyGuardState(const EEnemyGuardState GuardState);
	virtual EEnemyGuardState GetEnemyGuardState();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	const FName GuardStateKeyName = TEXT("GuardState");
	EEnemyGuardState EnemyGuardState = EEnemyGuardState::EEGS_NoGuard;


private: 
	
	
};
