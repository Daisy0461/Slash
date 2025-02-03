// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowingAOEAttack.generated.h"

class UBlackboardComponent;

UCLASS()
class SLASH_API UBTTask_FollowingAOEAttack : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UBlackboardComponent* BlackboardComp;
};
