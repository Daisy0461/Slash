// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FocusTarget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_FocusTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_FocusTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector FocusTargetKey;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
