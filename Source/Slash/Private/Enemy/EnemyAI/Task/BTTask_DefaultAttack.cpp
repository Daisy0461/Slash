// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_DefaultAttack.h"
#include "Enemy/Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_DefaultAttack::UBTTask_DefaultAttack()
{

}

EBTNodeResult::Type UBTTask_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    UE_LOG(LogTemp, Display, TEXT("In Default Attack"));
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
            if(BlackboardComp && BlackboardComp->GetValueAsBool("IsAttacking")){
                BlackboardComp->SetValueAsBool("IsAttacking", false); 
            }else{
                UE_LOG(LogTemp, Warning, TEXT("BTTask_DefaultAttack AttackFinished Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
            }
            
            if(OwnerComp.GetAIOwner()){
                UE_LOG(LogTemp, Display, TEXT("DefaultAttack Succeeded"));
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            }else{
                UE_LOG(LogTemp, Error, TEXT("OwnerComp is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
            }
        }
    );

    OwnerEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    //OwnerEnemy->ShortRangeAttack();
    OwnerEnemy->EnemyAutoAttack();
    UE_LOG(LogTemp, Display, TEXT("DefaultAttack InProgress"));
    return EBTNodeResult::InProgress; 
}