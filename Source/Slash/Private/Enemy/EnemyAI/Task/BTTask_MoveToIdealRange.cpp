// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_MoveToIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

// UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
// {

// }

// EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
// {
//     EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
//     APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
//     if(nullptr == ControllingPawn) return EBTNodeResult::Failed;

//     AEnemy* OwnerEnemy =  Cast<AEnemy>(ControllingPawn);
//     if(!OwnerEnemy) return EBTNodeResult::Failed;

//     UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
//     if(!AttackTargetObject) return EBTNodeResult::Failed;
//     AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);
//     if(!AttackTargetActor) return EBTNodeResult::Failed;

//     float IdealRange = BlackboardComp->GetValueAsObject(IdealRangeKey.SelectedKeyName);

// }


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
    NodeName = "Move To Ideal Range";
}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

    // Blackboard 컴포넌트에서 AttackTargetKey에 해당하는 값을 가져옴
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;
    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if(!AttackTargetObject) return EBTNodeResult::Failed;
    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);
    if(!AttackTargetActor) return EBTNodeResult::Failed;

    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);

    // AI가 TargetActor(AttackTargetActor)로 IdealRange 만큼의 거리를 두고 이동
    EPathFollowingRequestResult::Type MoveResult = OwnerComp.GetAIOwner()->MoveToActor(AttackTargetActor, IdealRange, true, true, false, true);

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        return EBTNodeResult::Failed;
    }

    //완료를 알릴 Delegate 필요함.

    return EBTNodeResult::InProgress;
}