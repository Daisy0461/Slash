// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTS_UpdateDistanceToAttackTarget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTS_UpdateDistanceToAttackTarget : public UBTService
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector DistanceToAttackTargetKey;
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
