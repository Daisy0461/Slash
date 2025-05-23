// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Interfaces/ParryInterface.h" 

AWarriorEnemyAIController::AWarriorEnemyAIController()
{
    
}

void AWarriorEnemyAIController::OnPossess(APawn* InPawn){
    Super::OnPossess(InPawn);
}

void AWarriorEnemyAIController::SetEnemyStateAsAttacking(AActor* AttackTarget)
{
    if(!BlackboardComponent) {
        UE_LOG(LogTemp, Display, TEXT("In SetEnemyStateAsAttacking BlackComponent can't find"));
        return;
    }

    if(IParryInterface* ParryCheckInterface = Cast<IParryInterface>(AttackTarget)){
        BlackboardComponent->SetValueAsObject(AttackTargetKeyName, AttackTarget);

        SetEnemyState(EEnemyState::EES_Attacking);
    }
}

void AWarriorEnemyAIController::SetEnemyStateAsHitting(AActor* AttackTarget)
{
    if(EnemyState != EEnemyState::EES_Dead){
        if(!BlackboardComponent) {
            UE_LOG(LogTemp, Display, TEXT("In SetEnemyStateAsAttacking BlackComponent can't find"));
            return;
        }
        BlackboardComponent->SetValueAsObject(AttackTargetKeyName, AttackTarget);

        SetEnemyState(EEnemyState::EES_Hitting);

    }
}

void AWarriorEnemyAIController::SetEnemyGuardState(const EEnemyGuardState GuardState)
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

EEnemyGuardState AWarriorEnemyAIController::GetEnemyGuardState()
{
    return static_cast<EEnemyGuardState>(BlackboardComponent->GetValueAsEnum(GuardStateKeyName));
}