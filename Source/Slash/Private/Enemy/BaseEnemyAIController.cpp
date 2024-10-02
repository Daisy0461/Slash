// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Enemy/Enemy.h"

ABaseEnemyAIController::ABaseEnemyAIController()
{

}

void ABaseEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
    Enemy = Cast<AEnemy>(InPawn);
    if(!Enemy){
        UE_LOG(LogTemp, Display, TEXT("AI Controller Cast Fail"));
        return;
    }

    AIPerceptionComponent = Enemy->GetAIPerceptionComponent();
    RunBehaviorTree(Enemy->GetBehaviorTree());
    SetEnemyState(EEnemyState::EES_Passive);

    BlackboardComponent = GetBlackboardComponent();
    if(!BlackboardComponent){
        UE_LOG(LogTemp, Display, TEXT("Blackboard Component is null"));
        return;
    }
    FName AttackRadiusKeyName = TEXT("AttackTarget");
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, Enemy->GetAttackRadius());
    FName DefendRadiusKeyName = TEXT("DefendRadius");
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, Enemy->GetDefendRadius());


}



void ABaseEnemyAIController::SetEnemyState(const EEnemyState State)
{
    BlackboardComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(State));
    EnemyState = State;
}

EEnemyState ABaseEnemyAIController::GetEnemyState() const
{
    return static_cast<EEnemyState>(BlackboardComponent->GetValueAsEnum(StateKeyName));
}
