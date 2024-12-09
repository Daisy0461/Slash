// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTD_IsHealthBelowThreshold.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTD_IsHealthBelowThreshold : public UBTDecorator
{
	GENERATED_BODY()
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const;
	UPROPERTY(EditAnywhere, Category = "Threshold")
    float HealthThreshold = 0.5;
};
