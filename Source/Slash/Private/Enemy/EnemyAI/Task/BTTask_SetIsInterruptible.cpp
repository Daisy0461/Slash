// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_SetIsInterruptible.h"
#include "Character/BaseCharacter.h"
#include "Enemy/Enemy.h"
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
    if(!ControllingPawn){
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_SetIsInterruptible Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    ABaseCharacter* OwnerBaseCharacter =  Cast<ABaseCharacter>(ControllingPawn);
    if(!OwnerBaseCharacter){
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_SetIsInterruptible BaseCharacter Cast Failed"));
        return EBTNodeResult::Failed;
    }

    AEnemy* OwnerEnemy = Cast<AEnemy>(OwnerBaseCharacter);
    if(!OwnerEnemy){
        UE_LOG(LogTemp, Warning, TEXT("UBTTask_SetIsInterruptible Enemy Cast Failed"));
        return EBTNodeResult::Failed;
    }

    OwnerBaseCharacter->SetIsInterruptible(bSetIsInterruptible);
    if(bSetIsInterruptible){        //방해한다.
        OwnerEnemy->SetReduceDamagePercent(0.f);
    }else{
        OwnerEnemy->SetReduceDamagePercent(ReduceDamagePercent);
    }
    return EBTNodeResult::Succeeded;
}

FString UBTTask_SetIsInterruptible::GetStaticDescription() const
{
    FString Print = bSetIsInterruptible ? TEXT("True") : TEXT("False");
    return Print;
}