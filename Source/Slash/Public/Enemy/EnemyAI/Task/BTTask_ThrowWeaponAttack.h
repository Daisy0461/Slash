// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ThrowWeaponAttack.generated.h"

class UBlackboardComponent;

UCLASS()
class SLASH_API UBTTask_ThrowWeaponAttack : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwerComp, uint8* NodeMemory) override;
protected:
	UBlackboardComponent* BlackboardComp;
};
