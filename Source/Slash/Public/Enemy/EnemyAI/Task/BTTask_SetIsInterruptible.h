// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetIsInterruptible.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_SetIsInterruptible : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetIsInterruptible();
	virtual FString GetStaticDescription() const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
    bool bSetIsInterruptible = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	float ReduceDamagePercent = 0.f;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
};
