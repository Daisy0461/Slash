// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_DefaultAttack.h"
#include "Enemy/EnemyInterface.h"
#include "Enemy/Enemy.h"
#include "AIController.h"

UBTTask_DefaultAttack::UBTTask_DefaultAttack()
{

}

EBTNodeResult::Type UBTTask_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_DefaultAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    AEnemy* OwnerEnemy =  Cast<AEnemy>(ControllingPawn);
    if(nullptr == OwnerEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_DefaultAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            //UE_LOG(LogTemp, Display, TEXT("Attack Succeeded"));
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OwnerEnemy->SetAIAttackDelegate(OnAttackFinished);
    OwnerEnemy->AttackByAI();
    return EBTNodeResult::InProgress; 
}