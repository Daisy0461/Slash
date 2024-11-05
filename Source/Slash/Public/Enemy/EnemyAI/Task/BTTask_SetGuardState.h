// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BTTask_SetGuardState.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_SetGuardState : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetGuardState();
	UPROPERTY(EditAnywhere)
	EEnemyGuardState EnemyGuardState = EEnemyGuardState::EEGS_NoGuard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector GuardStateKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
private:
	virtual FString GetGuardStateAsString(EEnemyGuardState GuardState) const;

	
};
