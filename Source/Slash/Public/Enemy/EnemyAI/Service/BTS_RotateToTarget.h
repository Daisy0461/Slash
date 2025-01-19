// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTS_RotateToTarget : public UBTService
{
	GENERATED_BODY()
	//FocusTarget하고 Allow Physic~~ RootMotion하면 돌아감...
public:
	virtual FString GetStaticDescription() const override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FBlackboardKeySelector AttackTargetKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
