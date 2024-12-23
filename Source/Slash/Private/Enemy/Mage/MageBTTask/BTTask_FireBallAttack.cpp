#include "Enemy/Mage/MageBTTask/BTTask_FireBallAttack.h"
#include "Enemy/Mage/MageEnemy.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_FireBallAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_FireUBTTask_FireBallAttack Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    AMageEnemy* OwnerMageEnemy =  Cast<AMageEnemy>(ControllingPawn);
    if(nullptr == OwnerMageEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_FireUBTTask_FireBallAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OwnerMageEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    OwnerMageEnemy->FireBallAttack();
    return EBTNodeResult::InProgress; 
}