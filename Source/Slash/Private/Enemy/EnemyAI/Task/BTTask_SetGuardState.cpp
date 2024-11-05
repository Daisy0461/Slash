// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_SetGuardState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetGuardState::UBTTask_SetGuardState()
{
    NodeName = "Set Guard State";
}

EBTNodeResult::Type UBTTask_SetGuardState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController에서 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // 블랙보드 키 값 업데이트 (State 변경)
    uint8 StateValue = static_cast<uint8>(EnemyGuardState);
    BlackboardComp->SetValueAsEnum(GuardStateKey.SelectedKeyName, StateValue);

    // 성공적으로 상태를 변경했다면 성공 반환
    return EBTNodeResult::Succeeded;
}

FString UBTTask_SetGuardState::GetGuardStateAsString(EEnemyGuardState GuardState) const
{
    switch (GuardState)
    {
    case EEnemyGuardState::EEGS_NoGuard:
        return TEXT("No Guarding");
    case EEnemyGuardState::EEGS_Guarding:
        return TEXT("Guarding");
    case EEnemyGuardState::EEGS_GuardComplete:
        return TEXT("Guard Complete");
    default:
        return TEXT("Unknown");
    }
}

// GetStaticDescription 함수를 새로운 GuardState에 맞게 수정
FString UBTTask_SetGuardState::GetStaticDescription() const
{
    return FString::Printf(TEXT("\nSetGuardState: %s"), *GetGuardStateAsString(EnemyGuardState));
}