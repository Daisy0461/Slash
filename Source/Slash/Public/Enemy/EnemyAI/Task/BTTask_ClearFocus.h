// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearFocus.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_ClearFocus : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_ClearFocus();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
};
