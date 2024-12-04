// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageBTTask/BTTask_TeleportNSDeactive.h"
#include "Enemy/Mage/MageEnemy.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTTask_TeleportNSDeactive::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;
    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;
    AMageEnemy* Mage = Cast<AMageEnemy>(Character);
    if(!Mage) return EBTNodeResult::Failed;
    //UE_LOG(LogTemp, Display, TEXT("Mage Cast Complete"));

    Mage->DeactivateTeleportNiagara();
    return EBTNodeResult::Succeeded;
}