// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FireBallAttack.generated.h"

class AEnemy;
class UBlackboardComponent;
class AAIController;

UCLASS()
class SLASH_API UBTTask_FireBallAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_FireBallAttack();
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    bool bIsBarrageAttack = false;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;

private:
	AEnemy* OwnerEnemy;
	UBlackboardComponent* BlackboardComp;
	AAIController* AIController;
};
