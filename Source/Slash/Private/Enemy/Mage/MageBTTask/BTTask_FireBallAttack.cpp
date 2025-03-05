#include "Enemy/Mage/MageBTTask/BTTask_FireBallAttack.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "BehaviorTree/BlackboardComponent.h"
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

    OwnerEnemy =  Cast<AEnemy>(ControllingPawn);
    if(nullptr == OwnerEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_FireUBTTask_FireBallAttack Cast Failed"));
        return EBTNodeResult::Failed;
    }

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
    if(bIsBarrageAttack){
        OwnerEnemy->GetEnemyFireBall()->EnemyFireBallAttack(EEnemyFireBallEnum::EFBE_BarrageFireBall);
    }else{
        OwnerEnemy->GetEnemyFireBall()->EnemyFireBallAttack(EEnemyFireBallEnum::EFBE_BasicFireBall);
    }

    return EBTNodeResult::InProgress; 
}

FString UBTTask_FireBallAttack::GetStaticDescription() const
{
    FString Print = bIsBarrageAttack ? TEXT("BarrageAttack") : TEXT("FireBallAttack");
    return Print;
}