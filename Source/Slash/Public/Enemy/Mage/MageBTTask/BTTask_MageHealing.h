// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MageHealing.generated.h"

class AEnemy;
class UBlackboardComponent;
class AAIController;

UCLASS()
class SLASH_API UBTTask_MageHealing : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	AEnemy* OwnerEnemy;
	UBlackboardComponent* BlackboardComp;
	AAIController* AIController;
};
