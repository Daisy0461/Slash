// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Decorator/BTD_IsWithInIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsWithInIdealRange::UBTD_IsWithInIdealRange()
{

}

bool UBTD_IsWithInIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn) return true;
    
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return true;

    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);
    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    AAIController* AIController = OwnerComp.GetAIOwner();

    float AttackTargetDistance = AttackTargetActor->GetDistanceTo(ControllingPawn);

    if((AttackTargetDistance - ErrorMarin) <= IdealRange){
        //return true;
        return false;
    }

    //return false;
    return true;
}

FString UBTD_IsWithInIdealRange::GetStaticDescription() const
{
    return FString::Printf(TEXT("\nAttackTargetKey: %s\nIdealRangeKey: %s"), *AttackTargetKey.SelectedKeyName.ToString(), *IdealRangeKey.SelectedKeyName.ToString());
}