// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorJumpAttack.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_WarriorJumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_FireUBTTask_WarriorJumpAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    AWarriorEnemy* OwnerWarriorEnemy =  Cast<AWarriorEnemy>(ControllingPawn);
    if(nullptr == OwnerWarriorEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_FireUBTTask_WarriorJumpAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OwnerWarriorEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    OwnerWarriorEnemy->LongRangeAttack_Jump();
    return EBTNodeResult::InProgress; 
}
