// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void AWarlordEnemyAIController::SetEnemyGuardState(const EEnemyGuardState GuardState)
{
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("BlackboardComponent not found"));
        return;
    }

    uint8 StateValue = static_cast<uint8>(GuardState);
    BlackboardComponent->SetValueAsEnum(GuardStateKeyName, StateValue);
    EnemyGuardState = GuardState;
}

EEnemyGuardState AWarlordEnemyAIController::GetEnemyGuardState()
{
    return static_cast<EEnemyGuardState>(BlackboardComponent->GetValueAsEnum(GuardStateKeyName));
}