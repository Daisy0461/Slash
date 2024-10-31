// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_EnemyStateCheck.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTS_EnemyStateCheck : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_EnemyStateCheck();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
