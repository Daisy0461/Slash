// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WarriorSpinningAttack.generated.h"

class AWarriorEnemy;
class UBlackboardComponent;

UCLASS()
class SLASH_API UBTTask_WarriorSpinningAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;

private:
	AWarriorEnemy* OwnerWarriorEnemy;
	UBlackboardComponent* BlackboardComp;
	AActor* AttackTarget;
};
