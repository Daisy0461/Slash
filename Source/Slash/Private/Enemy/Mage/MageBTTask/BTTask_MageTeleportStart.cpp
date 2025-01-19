// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageBTTask/BTTask_MageTeleportStart.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_MageTeleportStart::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;
    AEnemy* Enemy = Cast<AEnemy>(Character);
    if(!Enemy) return EBTNodeResult::Failed;
    //UE_LOG(LogTemp, Display, TEXT("Enemy Cast Complete"));

    Enemy->EnemyTeleport(EEnemyTeleportEnum::EETE_StartTeleport);
    return EBTNodeResult::Succeeded;
}
