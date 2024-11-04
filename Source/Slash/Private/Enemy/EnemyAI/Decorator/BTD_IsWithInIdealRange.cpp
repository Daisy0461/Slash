#include "Enemy/EnemyAI/Decorator/BTD_IsWithInIdealRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTD_IsWithInIdealRange::UBTD_IsWithInIdealRange()
{
    bNotifyTick = true;
    bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
    LastConditionValue = false;
}

void UBTD_IsWithInIdealRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Decorator가 활성화될 때 호출
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
    bNotifyTick = true;
}

void UBTD_IsWithInIdealRange::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Decorator가 비활성화될 때 호출
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
    bNotifyTick = false;
}

void UBTD_IsWithInIdealRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    bool bConditionValue = CalculateRawConditionValue(OwnerComp, NodeMemory);

    if (bConditionValue != LastConditionValue) {
        OwnerComp.RequestExecution(this);
    }

    LastConditionValue = bConditionValue;
}

bool UBTD_IsWithInIdealRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
    bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn) return true;
    
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return true;

    UObject* AttackTargetObject = BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName);
    if(!AttackTargetObject){
        UE_LOG(LogTemp, Warning, TEXT("Cann't find AttackTargetKey Object"));
        return false;
    }

    AActor* AttackTargetActor = Cast<AActor>(AttackTargetObject);       //여기서 viking을 들고 옴.
    if(!AttackTargetActor){
        UE_LOG(LogTemp, Warning, TEXT("AttackTargetKey cast to Actor Fail"));
        return false;
    }
    float IdealRange = BlackboardComp->GetValueAsFloat(IdealRangeKey.SelectedKeyName);
    AAIController* AIController = OwnerComp.GetAIOwner();

    float AttackTargetDistance = AttackTargetActor->GetDistanceTo(ControllingPawn);

    if((AttackTargetDistance - ErrorMarin) <= IdealRange){
        return false;
    }

    return true;
}


FString UBTD_IsWithInIdealRange::GetStaticDescription() const
{
    return FString::Printf(TEXT("\nAttackTargetKey: %s\nIdealRangeKey: %s"), *AttackTargetKey.SelectedKeyName.ToString(), *IdealRangeKey.SelectedKeyName.ToString());
}
