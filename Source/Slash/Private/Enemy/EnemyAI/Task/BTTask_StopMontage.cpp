// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_StopMontage.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
UBTTask_StopMontage::UBTTask_StopMontage()
{
    NodeName = "Stop Animation Montage";
}

EBTNodeResult::Type UBTTask_StopMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character) return EBTNodeResult::Failed;

    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance) return EBTNodeResult::Failed;

    // Montage가 설정되어 있으면 해당 Montage를 중지, 없으면 현재 재생 중인 모든 Montage를 중지
    if (MontageToStop)
    {
        AnimInstance->Montage_Stop(0.2f, MontageToStop);  // 지정한 Montage를 중지
    }
    else
    {
        AnimInstance->Montage_Stop(0.2f);  // 모든 Montage 중지
    }

    return EBTNodeResult::Succeeded;  // Task 완료
}