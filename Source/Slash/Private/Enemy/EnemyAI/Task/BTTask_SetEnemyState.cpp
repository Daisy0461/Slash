// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Task/BTTask_SetEnemyState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetEnemyState::UBTTask_SetEnemyState()
{
    // 여기서 블랙보드의 기본 키를 설정할 수 있습니다 (예: "State"라는 블랙보드 키)
    NodeName = "Set Enemy AI State";
}

EBTNodeResult::Type UBTTask_SetEnemyState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AIController에서 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    // 블랙보드 키 값 업데이트 (State 변경)
    if(static_cast<EEnemyState>(BlackboardComp->GetValueAsEnum(StateKey.SelectedKeyName)) == EEnemyState::EES_Dead){
        //UE_LOG(LogTemp, Display, TEXT("Set State Task == Dead"));
        return EBTNodeResult::Succeeded;
    }
    //UE_LOG(LogTemp, Display, TEXT("Set State Task != Dead"));
    uint8 StateValue = static_cast<uint8>(EnemyState);
    BlackboardComp->SetValueAsEnum(StateKey.SelectedKeyName, StateValue);


    // 성공적으로 상태를 변경했다면 성공 반환
    return EBTNodeResult::Succeeded;
}

FString UBTTask_SetEnemyState::GetEnemyStateAsString(EEnemyState State) const
{
    switch (State)
    {
    case EEnemyState::EES_Passive:
        return TEXT("Passive");
    case EEnemyState::EES_Investing:
        return TEXT("Investing");
    case EEnemyState::EES_Chasing:
        return TEXT("Chasing");
    case EEnemyState::EES_Strafing:
        return TEXT("Strafing");
    case EEnemyState::EES_Attacking:
        return TEXT("Attacking");
    case EEnemyState::EES_Parried:
        return TEXT("Parried");
    case EEnemyState::EES_Hitting:
        return TEXT("Hitting");
    case EEnemyState::EES_Dead:
        return TEXT("Dead");
    case EEnemyState::EES_NoState:
        return TEXT("No State");
    default:
        return TEXT("Unknown");
    }
}

FString UBTTask_SetEnemyState::GetStaticDescription() const
{
    return  FString::Printf(TEXT("SetEnemyState: %s"), *GetEnemyStateAsString(EnemyState));
}