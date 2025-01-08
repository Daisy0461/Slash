#include "Enemy/Mage/MageBTTask/BTTask_FireBallAttack.h"
#include "Enemy/Mage/MageEnemy.h"
#include "AIController.h"
UBTTask_FireBallAttack::UBTTask_FireBallAttack()
{
    if(bIsBarrageAttack){
        NodeName = TEXT("BarrageAttack");
    }else{
        NodeName = TEXT("FireBallAttack");
    }
}

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

    if(bIsBarrageAttack){
        OwnerMageEnemy->BarrageAttack();
        
    }else{
        OwnerMageEnemy->FireBallAttack();
    }

    return EBTNodeResult::InProgress; 
}

FString UBTTask_FireBallAttack::GetStaticDescription() const
{
    FString Print = bIsBarrageAttack ? TEXT("BarrageAttack") : TEXT("FireBallAttack");
    return Print;
}