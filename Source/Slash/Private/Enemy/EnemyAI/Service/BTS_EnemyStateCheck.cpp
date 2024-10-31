// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Service/BTS_EnemyStateCheck.h"

UBTS_EnemyStateCheck::UBTS_EnemyStateCheck()
{

}

void UBTS_EnemyStateCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // AAIController* AIC = OwnerComp.GetAIOwner();
    // if(!AIC){
    //     UE_LOG(LogTemp, Display, TEXT("AAIC cant find in CheckAttackTargetIsDead"));
    //     return;
    // }

    // UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();
    // if(!BlackboardComp)
    // {
    //     UE_LOG(LogTemp, Display, TEXT("BlackboardComp cant find in CheckAttackTargetIsDead"));
    //     return;
    // }

    // AVikingCharacter* Viking = Cast<AVikingCharacter>(BlackboardComp->GetValueAsObject(AttackTargetKeyName));
    // if(!Viking){
    //     UE_LOG(LogTemp, Display, TEXT("Viking Cast Failed"));
    //     return;
    // }

    // if(Viking->GetActionState() == EActionState::EAS_Dead)
    // {
    //     ABaseEnemyAIController* BAIC = Cast<ABaseEnemyAIController>(AIC);
    //     if(BAIC){
    //         BAIC->SetEnemyStateAsPassive();
    //     }
    // }
}