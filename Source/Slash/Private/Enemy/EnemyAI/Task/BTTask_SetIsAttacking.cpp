// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_SetIsAttacking.h"
#include "Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_SetIsAttacking::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_DefaultAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp){
        UE_LOG(LogTemp, Warning, TEXT("Get Blackboard Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    BlackboardComp->SetValueAsBool("IsAttacking", bIsAttacking);

    return EBTNodeResult::Succeeded; 
}

FString UBTTask_SetIsAttacking::GetStaticDescription() const
{
    FString IsAttacking = bIsAttacking ? TEXT("True") : TEXT("False");
    return FString::Printf(TEXT("Set IsAttacking to %s"), *IsAttacking);
}