// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageBTTask/BTTask_MageHealing.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_MageHealing::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;
    OwnerEnemy = Cast<AEnemy>(Character);
    if(!OwnerEnemy) return EBTNodeResult::Failed;
    //UE_LOG(LogTemp, Display, TEXT("Mage Cast Complete"));
    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp) return EBTNodeResult::Failed;

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

    OwnerEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    OwnerEnemy->EnemyAOEAttack(EEnemyAOEAttackEnum::EEAA_HealingArea);
    return EBTNodeResult::Succeeded;
}