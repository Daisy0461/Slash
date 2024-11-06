// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Decorator/BTD_IsBeyondIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsBeyondIdealRange::UBTD_IsBeyondIdealRange()
{
    bNotifyTick = true;
    bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
    LastConditionValue = false;
}

void UBTD_IsBeyondIdealRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Decorator가 활성화될 때 호출
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
    bNotifyTick = true;
}

void UBTD_IsBeyondIdealRange::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Decorator가 비활성화될 때 호출
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
    bNotifyTick = true;
}

void UBTD_IsBeyondIdealRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    //OwnerComp.RequestExecution(this);
    bool bConditionValue = CalculateRawConditionValue(OwnerComp, NodeMemory);

    if (bConditionValue != LastConditionValue) {
        OwnerComp.RequestExecution(this);
    }

    LastConditionValue = bConditionValue;
}

bool UBTD_IsBeyondIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    //bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn) return true;
    
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return true;

    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);       //여기서 viking을 들고 옴.
    if(!AttackTargetActor){
        UE_LOG(LogTemp, Warning, TEXT("AttackTargetKey cast to Actor Fail"));
        return true;
    }

    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    float AttackTargetDistance = AttackTargetActor->GetDistanceTo(ControllingPawn);

    return (AttackTargetDistance - ErrorMarin) < IdealRange;
}


FString UBTD_IsBeyondIdealRange::GetStaticDescription() const
{
    return FString::Printf(TEXT("\nAttackTargetKey: %s\nIdealRangeKey: %s"), *AttackTargetKey.SelectedKeyName.ToString(), *IdealRangeKey.SelectedKeyName.ToString());
}