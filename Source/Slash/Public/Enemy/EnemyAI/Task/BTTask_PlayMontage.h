// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BTTask_PlayMontage.generated.h"

UCLASS()
class SLASH_API UBTTask_PlayMontage : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_PlayMontage();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* MontageToPlay;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
	UFUNCTION()
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
    UBehaviorTreeComponent* BehaviorTreeComponent;
    UPROPERTY()
    UAnimInstance* AnimInstance;
	
};
