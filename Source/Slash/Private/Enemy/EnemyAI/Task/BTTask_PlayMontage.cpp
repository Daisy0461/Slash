// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_PlayMontage.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_PlayMontage::UBTTask_PlayMontage()
{
    NodeName = "Play Animation Montage";
}

EBTNodeResult::Type UBTTask_PlayMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    BehaviorTreeComponent = &OwnerComp;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;

    AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance || !MontageToPlay) return EBTNodeResult::Failed;

    AnimInstance->Montage_Play(MontageToPlay);
    AnimInstance->OnMontageEnded.AddDynamic(this, &UBTTask_PlayMontage::OnMontageEnded);

    return EBTNodeResult::InProgress; 
}

void UBTTask_PlayMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    if (AnimInstance && MontageToPlay)
    {
        AnimInstance->Montage_Stop(0.2f, MontageToPlay);
    }

    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UBTTask_PlayMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == MontageToPlay && BehaviorTreeComponent)
    {
        FinishLatentTask(*BehaviorTreeComponent, EBTNodeResult::Succeeded);
        if (AnimInstance)
        {
            AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBTTask_PlayMontage::OnMontageEnded);
        }
    }
}