// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Enemy/AttackStruct.h"
#include "VikingGameState.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AVikingGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	TArray<FAttackInfo> ActiveAttacks;
	void AddAttack(FAttackInfo AttackInfo);
	void RemoveAttack(FAttackInfo AttackInfo);
	
};
