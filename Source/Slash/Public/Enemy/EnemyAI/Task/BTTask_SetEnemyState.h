// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BTTask_SetEnemyState.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_SetEnemyState : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_SetEnemyState();
	UPROPERTY(EditAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Passive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector StateKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
private:
	virtual FString GetEnemyStateAsString(EEnemyState State) const;
	
};
