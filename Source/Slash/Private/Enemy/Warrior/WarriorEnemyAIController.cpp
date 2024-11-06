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
        UE_LOG(LogTemp, Display, TEXT("ParryInterface Cast"));
        BlackboardComponent->SetValueAsObject(AttackTargetKeyName, AttackTarget);
        AttackTargetActor = AttackTarget;
        if(!AttackTargetActor){
            UE_LOG(LogTemp, Display, TEXT("AttackTargetActor = fail"));
        }
        SetEnemyState(EEnemyState::EES_Attacking);
    }
}

void AWarriorEnemyAIController::SetEnemyStateAsHitting(AActor* AttackTarget)
{
    if(EnemyState != EEnemyState::EES_Dead){
        //UE_LOG(LogTemp, Warning, TEXT("Not Dead Hitting"));
        if(!BlackboardComponent) {
            UE_LOG(LogTemp, Display, TEXT("In SetEnemyStateAsAttacking BlackComponent can't find"));
            return;
        }

        if(!AttackTargetActor || AttackTargetActor == nullptr){             //지금 AttackTarget이 누군지 모른다. -> Guard 불가능
            UE_LOG(LogTemp, Display, TEXT("Set Hitting"));
            AttackTargetActor = AttackTarget;
            BlackboardComponent->SetValueAsObject(AttackTargetKeyName, AttackTarget);
        }
        SetEnemyState(EEnemyState::EES_Hitting);

    }else{
        //UE_LOG(LogTemp, Warning, TEXT("Dead Hitting"));
    }
}

void AWarriorEnemyAIController::SetEnemyGuardState(const EEnemyGuardState GuardState)
{
    if (!BlackboardComponent)
    {
        UE_LOG(LogTemp, Display, TEXT("BlackboardComponent not found"));
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("In Change"));
    uint8 StateValue = static_cast<uint8>(GuardState);
    BlackboardComponent->SetValueAsEnum(GuardStateKeyName, StateValue);
    EnemyGuardState = GuardState;
}

EEnemyGuardState AWarriorEnemyAIController::GetEnemyGuardState()
{
    return static_cast<EEnemyGuardState>(BlackboardComponent->GetValueAsEnum(GuardStateKeyName));
}