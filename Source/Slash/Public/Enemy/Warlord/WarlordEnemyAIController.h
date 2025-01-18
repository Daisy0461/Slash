// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "WarlordEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AWarlordEnemyAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()
public:
	virtual void SetEnemyGuardState(const EEnemyGuardState GuardState);
	virtual EEnemyGuardState GetEnemyGuardState();
	
protected:
	const FName GuardStateKeyName = TEXT("GuardState");
	EEnemyGuardState EnemyGuardState = EEnemyGuardState::EEGS_NoGuard;


};
