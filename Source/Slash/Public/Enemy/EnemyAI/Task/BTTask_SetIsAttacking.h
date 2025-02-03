// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetIsAttacking.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_SetIsAttacking : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere)
	bool bIsAttacking = true;
private:
	virtual FString GetStaticDescription() const;
};
   