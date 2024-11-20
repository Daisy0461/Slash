// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DodgeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDodgeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IDodgeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	bool isInEnemyAttackArea = false;

	virtual void SetIsInEnemyAttackArea(bool isIn) = 0;
	virtual bool GetIsInEnemyAttackArea();
};
