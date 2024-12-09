// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Decorator/BTD_IsHealthBelowThreshold.h"
#include "AIController.h"
#include "Character/BaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

bool UBTD_IsHealthBelowThreshold::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// Decorator가 활성화될 때 호출
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn) return false;

    ABaseCharacter* ControllingCharacterCharacter = Cast<ABaseCharacter>(ControllingPawn);
    if (!ControllingCharacterCharacter) return false;

    return ControllingCharacterCharacter->GetCharacterHealthPercent() < HealthThreshold;
}

FString UBTD_IsHealthBelowThreshold::GetStaticDescription() const
{
    return FString::Printf(TEXT("\nHealth Threshold: %f"), HealthThreshold);
}
