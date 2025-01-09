// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WarriorAOEAttack.generated.h"

class AWarriorEnemy;
class UBlackboardComponent;
class AAIController;

UCLASS()
class SLASH_API UBTTask_WarriorAOEAttack : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	AWarriorEnemy* OwnerWarriorEnemy;
	UBlackboardComponent* BlackboardComp;
	AAIController* AIController;
};
