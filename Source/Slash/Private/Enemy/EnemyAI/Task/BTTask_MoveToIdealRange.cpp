#include "Enemy/EnemyAI/Task/BTTask_MoveToIdealRange.h"
#include "Enemy/EnemyInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Actor.h"


UBTTask_MoveToIdealRange::UBTTask_MoveToIdealRange()
{
    NodeName = "Move To Ideal Range";
}

EBTNodeResult::Type UBTTask_MoveToIdealRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return EBTNodeResult::Failed;

    AAIController* AIController = OwnerComp.GetAIOwner();
    if(!AIController) return EBTNodeResult::Failed;
   
    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if (!AttackTargetObject) return EBTNodeResult::Failed;
    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);
    if (!AttackTargetActor) return EBTNodeResult::Failed;

    FVector const AttackTargetActorLocation = AttackTargetActor->GetActorLocation();

    UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController, AttackTargetActorLocation);

    return EBTNodeResult::Succeeded;
}


FString UBTTask_MoveToIdealRange::GetStaticDescription() const
{ 
    return FString::Printf(TEXT("\nAttackTargetKey: %s"), *AttackTargetKey.SelectedKeyName.ToString());
}
