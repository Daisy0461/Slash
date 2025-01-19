// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"
#include "EnemyTeleportInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyTeleportInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SLASH_API IEnemyTeleportInterface
{
	GENERATED_BODY()
	virtual void EnemyTeleport(EEnemyTeleportEnum TeleportFunctionType) = 0;
};
