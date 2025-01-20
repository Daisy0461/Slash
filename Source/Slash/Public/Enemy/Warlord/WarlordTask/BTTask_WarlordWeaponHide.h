// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WarlordWeaponHide.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UBTTask_WarlordWeaponHide : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	bool bIsHideWeapon = true;
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
