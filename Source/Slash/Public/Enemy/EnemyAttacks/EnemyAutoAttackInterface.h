// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyAutoAttackInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyAutoAttackInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IEnemyAutoAttackInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EnemyAutoAttack(bool bIsRandom, int32 SectionNum) = 0;
};
