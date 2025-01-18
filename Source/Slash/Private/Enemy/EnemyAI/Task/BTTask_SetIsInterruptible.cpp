// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_SetIsInterruptible.h"
#include "Character/BaseCharacter.h"
#include "AIController.h"

UBTTask_SetIsInterruptible::UBTTask_SetIsInterruptible()
{
    if(bSetIsInterruptible){
        NodeName = TEXT("Set Interruptible");
    }else{
        NodeName = TEXT("Set Not Interruptible");
    }
}

EBTNodeResult::Type UBTTask_SetIsInterruptible::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Warning, TEXT("BTTask_FireUBTTask_FireBallAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* OwnerBaseCharacter =  Cast<ABaseCharacter>(ControllingPawn);
    if(nullptr == OwnerBaseCharacter){
        UE_LOG(LogTemp, Warning, TEXT("BTTask_FireUBTTask_FireBallAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

    OwnerBaseCharacter->SetIsInterruptible(bSetIsInterruptible);

    return EBTNodeResult::Succeeded;
}

FString UBTTask_SetIsInterruptible::GetStaticDescription() const
{
    FString Print = bSetIsInterruptible ? TEXT("True") : TEXT("False");
    return Print;
}