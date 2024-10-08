// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTS_CheckAttackTargetIsDead.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTS_CheckAttackTargetIsDead : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_CheckAttackTargetIsDead();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
	FName AttackTargetKeyName = TEXT("AttackTarget");
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	
};
