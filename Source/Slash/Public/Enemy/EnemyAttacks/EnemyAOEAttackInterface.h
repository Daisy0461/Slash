// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackEnum.h"
#include "EnemyAOEAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyAOEAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class SLASH_API IEnemyAOEAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType) = 0;
};
