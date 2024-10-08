#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToIdealRange.generated.h"

/**
 * Behavior Tree Task to move AI to an ideal range from a target actor.
 */
// class UBlackboardComponent;
// class AAIController;
// class UPathFollowingComponent;
// class AActor;

UCLASS()
class SLASH_API UBTTask_MoveToIdealRange : public UBTTaskNode
{
    GENERATED_BODY()
public:
    UBTTask_MoveToIdealRange();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FBlackboardKeySelector AttackTargetKey;

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual FString GetStaticDescription() const override;

};
