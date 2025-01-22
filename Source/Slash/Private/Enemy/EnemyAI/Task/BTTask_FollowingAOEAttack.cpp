// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_FollowingAOEAttack.h"
#include "Enemy/Enemy.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_FollowingAOEAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;
    AEnemy* OwnerEnemy = Cast<AEnemy>(Character);
    if(!OwnerEnemy) return EBTNodeResult::Failed;
    //UE_LOG(LogTemp, Display, TEXT("OwnerEnemy Cast Complete"));

    OwnerEnemy->EnemyAOEAttack(EEnemyAOEAttackEnum::EEAA_FollowingAreaAttack);
    return EBTNodeResult::Succeeded;
}