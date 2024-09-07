// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "Enemy/Enemy.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{

}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

    OwnerEnemy->SetMovementSpeedEnum(EEnemyMovementSpeed);

    return EBTNodeResult::Succeeded;
}