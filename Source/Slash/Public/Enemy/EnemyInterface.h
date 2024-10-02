// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

class AActor;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DELEGATE(FAIEnemyAttackFinished);
DECLARE_DELEGATE(FAIEnemyMoveFinished)

class SLASH_API IEnemyInterface
{
	GENERATED_BODY()
public:
	virtual AActor* GetPatrolRoute() const = 0;
	virtual void PlayStunMontage() = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	//Attack
	virtual void SetAIAttackDelegate(const FAIEnemyAttackFinished& InOnAttackFinished) = 0;
	virtual void AttackByAI() = 0;

	//ParryStun
	FAIEnemyAttackFinished OnAttackFinished;
	FAIEnemyMoveFinished OnMoveFinished;
};
