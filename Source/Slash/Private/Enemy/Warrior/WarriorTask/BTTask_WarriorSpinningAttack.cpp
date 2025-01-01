// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorSpinningAttack.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_WarriorSpinningAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn){
        UE_LOG(LogTemp, Warning, TEXT("OwnerComp's ControllingPawn cast Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp){
        UE_LOG(LogTemp, Warning, TEXT("Get Blackboard Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    OwnerWarriorEnemy =  Cast<AWarriorEnemy>(ControllingPawn);
    if(!OwnerWarriorEnemy){
        UE_LOG(LogTemp, Warning, TEXT("Cast to WarriorEnemy Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            if(BlackboardComp && BlackboardComp->GetValueAsBool("IsAttacking")){
                BlackboardComp->SetValueAsBool("IsAttacking", false);
            }
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OwnerWarriorEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    OwnerWarriorEnemy->LongRangeAttack_Spinning();
    return EBTNodeResult::InProgress; 
}