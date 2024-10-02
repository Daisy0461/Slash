// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BaseEnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
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


    RunBehaviorTree(Enemy->GetBehaviorTree());
    SetEnemyState(EEnemyState::EES_Passive);

    UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
    if(!BlackboardComponent){
        UE_LOG(LogTemp, Display, TEXT("Blackboard Component is null"));
        return;
    }
    FName AttackRadiusKeyName = TEXT("AttackTarget");
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, 150.f);
    FName DefendRadiusKeyName = TEXT("DefendRadius");
    BlackboardComponent->SetValueAsFloat(AttackRadiusKeyName, 500.f);
}



void ABaseEnemyAIController::SetEnemyState(EEnemyState State)
{
    EnemyState = State;
}