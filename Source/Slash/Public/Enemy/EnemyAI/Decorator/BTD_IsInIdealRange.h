// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_Blackboard.h"
#include "BTD_IsInIdealRange.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTD_IsInIdealRange : public UBTDecorator_Blackboard
{
	GENERATED_BODY()
public:
	UBTD_IsInIdealRange();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector IdealRangeKey;
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	float ErrorMarin = 30.f;
};
