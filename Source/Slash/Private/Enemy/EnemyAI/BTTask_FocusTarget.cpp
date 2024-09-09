// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/BTTask_FocusTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_FocusTarget::UBTTask_FocusTarget()
{

}

EBTNodeResult::Type UBTTask_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        UE_LOG(LogTemp, Display, TEXT("BTTask_Focus Can't find BlackboardComp"));
        return EBTNodeResult::Failed;
    }

    //BP에는 GetValueAsActor가 있는데 CPP엔 없다. 신기하군.
    //AActor* FocusTargetActor = BlackboardComp->GetValueAsActor(FocusTargetKey.SelectedKeyName);


    UObject* FocusTargetObject = BlackboardComp->GetValueAsObject(FocusTargetKey.SelectedKeyName);
    AActor* FocusTargetActor = Cast<AActor>(FocusTargetObject);
    AAIController* AIController = OwnerComp.GetAIOwner();

    if (FocusTargetActor && AIController)
    {
        AIController->SetFocus(FocusTargetActor);
        //UE_LOG(LogTemp, Display, TEXT("FocusTargetActor: %s"), *FocusTargetActor->GetName()); 
        return EBTNodeResult::Succeeded;
    }
    else
    {
        UE_LOG(LogTemp, Display, TEXT("BTTask_Focus Can't find FocusTarget or AIController"));   
        return EBTNodeResult::Failed;
    }
}

FString UBTTask_FocusTarget::GetStaticDescription() const
{ 
    return FString::Printf(TEXT("FocusTargetKey: %s"), *FocusTargetKey.SelectedKeyName.ToString());
}