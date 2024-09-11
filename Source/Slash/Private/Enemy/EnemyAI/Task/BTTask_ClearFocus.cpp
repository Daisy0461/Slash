#include "Enemy/EnemyAI/Task/BTTask_ClearFocus.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ClearFocus::UBTTask_ClearFocus()
{
    NodeName = "Clear AI Focus";
}

EBTNodeResult::Type UBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        AIController->ClearFocus(EAIFocusPriority::Gameplay);

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return EBTNodeResult::Succeeded;
    }
    else
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return EBTNodeResult::Failed;
    }
}
