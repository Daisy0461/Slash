// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_ThrowWeaponAttack.h"
#include "Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_ThrowWeaponAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_DefaultAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }


    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp){
        UE_LOG(LogTemp, Warning, TEXT("Get Blackboard Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    AEnemy* OwnerEnemy = Cast<AEnemy>(ControllingPawn);
    if(nullptr == OwnerEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_DefaultAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {            
            if(OwnerComp.GetAIOwner()){
                //UE_LOG(LogTemp, Display, TEXT("ThrowWeaponAttack Success"));
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }else{
                UE_LOG(LogTemp, Error, TEXT("OwnerComp is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
            }
        }
    );

    OwnerEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    OwnerEnemy->EnemyThrowWeaponAttack();
    return EBTNodeResult::InProgress; 
}