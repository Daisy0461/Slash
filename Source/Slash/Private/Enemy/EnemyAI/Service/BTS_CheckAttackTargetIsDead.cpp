#include "Enemy/EnemyAI/Service/BTS_CheckAttackTargetIsDead.h"
#include "AIController.h"
#include "Character/VikingCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy/BaseEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_CheckAttackTargetIsDead::UBTS_CheckAttackTargetIsDead()
{

}

void UBTS_CheckAttackTargetIsDead::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    if(!AIC){
        UE_LOG(LogTemp, Display, TEXT("AAIC cant find in CheckAttackTargetIsDead"));
        return;
    }

    UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
    if(!BlackboardComp)
    {
        UE_LOG(LogTemp, Display, TEXT("BlackboardComp cant find in CheckAttackTargetIsDead"));
        return;
    }

    AVikingCharacter* Viking = Cast<AVikingCharacter>(BlackboardComp->GetValueAsObject(AttackTargetKeyName));
    if(!Viking){
        UE_LOG(LogTemp, Display, TEXT("Viking Cast Failed"));
        return;
    }

    if(Viking->GetActionState() == EActionState::EAS_Dead)
    {
        ABaseEnemyAIController* BAIC = Cast<ABaseEnemyAIController>(AIC);
        if(BAIC){
            BAIC->SetEnemyStateAsPassive();
        }
    }
}
