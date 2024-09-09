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

    OwnerEnemy->SetMovementSpeedEnum(EnemyMovementSpeed);

    return EBTNodeResult::Succeeded;
}

FString UBTTask_SetMovementSpeed::GetMovementSpeedAsString(EEnemyMovementSpeed MovementSpeed) const
{
    switch (MovementSpeed)
    {
    case EEnemyMovementSpeed::EEMS_Idle:
        return TEXT("Idle");
    case EEnemyMovementSpeed::EEMS_Walk:
        return TEXT("Walk");
    case EEnemyMovementSpeed::EEMS_Jogging:
        return TEXT("Jogging");
    case EEnemyMovementSpeed::EEMS_Sprinting:
        return TEXT("Sprinting");
    default:
        return TEXT("Idle");
    }
}

FString UBTTask_SetMovementSpeed::GetStaticDescription() const
{
    return FString::Printf(TEXT("SetMovementSpeed: %s"), *GetMovementSpeedAsString(EnemyMovementSpeed));
}