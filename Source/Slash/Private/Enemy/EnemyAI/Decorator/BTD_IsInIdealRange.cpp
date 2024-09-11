// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Decorator/BTD_IsInIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsInIdealRange::UBTD_IsInIdealRange()
{

}

bool UBTD_IsInIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn) return false;
    
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return false;

    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);
    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    AAIController* AIController = OwnerComp.GetAIOwner();

    float AttackTargetDistance = AttackTargetActor->GetDistanceTo(ControllingPawn);

    if((AttackTargetDistance - ErrorMarin) <= IdealRange){
        return true;
    }

    return false;
}

