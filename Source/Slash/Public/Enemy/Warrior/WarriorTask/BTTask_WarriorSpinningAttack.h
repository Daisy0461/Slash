// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WarriorSpinningAttack.generated.h"

class AEnemy;
class UBlackboardComponent;
class AAIController;

UCLASS()
class SLASH_API UBTTask_WarriorSpinningAttack : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_WarriorSpinningAttack();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	virtual void ChaseToTarget();
	virtual void RotateToTarget();
	FRotator OriginRotation;

private:
	AEnemy* OwnerEnemy;
	UBlackboardComponent* BlackboardComp;
	AActor* AttackTarget;
	AAIController* AIController;
};
