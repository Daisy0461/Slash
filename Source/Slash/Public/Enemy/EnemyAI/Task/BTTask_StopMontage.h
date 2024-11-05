// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopMontage.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_StopMontage : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_StopMontage();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* MontageToStop;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
