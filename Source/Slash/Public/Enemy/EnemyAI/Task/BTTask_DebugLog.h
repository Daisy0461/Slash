// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DebugLog.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_DebugLog : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_DebugLog();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FString LogMessage;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
