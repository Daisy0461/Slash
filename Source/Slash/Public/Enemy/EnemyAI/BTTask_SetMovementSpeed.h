// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "BTTask_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetMovementSpeed();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere)
	EEnemyMovementSpeed EEnemyMovementSpeed = EEnemyMovementSpeed::EEMS_Sprinting;
};