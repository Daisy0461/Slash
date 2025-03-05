// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageBTTask/BTTask_MageAOEAttack.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_MageAOEAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;
    OwnerEnemy = Cast<AEnemy>(Character);
    if(!OwnerEnemy) return EBTNodeResult::Failed;

    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp) return EBTNodeResult::Failed;
    //UE_LOG(LogTemp, Display, TEXT("OwnerEnemy Cast Complete"));

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
    OwnerEnemy->EnemyAOEAttack(EEnemyAOEAttackEnum::EEAA_MagicAreaAttack);
    
    return EBTNodeResult::Succeeded;
}
