// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorJumpAttack.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_WarriorJumpAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn){
        UE_LOG(LogTemp, Warning, TEXT("OwnerComp's ControllingPawn cast Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp){
        UE_LOG(LogTemp, Warning, TEXT("Get Blackboard Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    OwnerWarriorEnemy =  Cast<AWarriorEnemy>(ControllingPawn);
    if(!OwnerWarriorEnemy){
        UE_LOG(LogTemp, Warning, TEXT("Cast to WarriorEnemy Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OwnerWarriorEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    OwnerWarriorEnemy->LongRangeAttack_Jump();
    return EBTNodeResult::InProgress; 
}

void UBTTask_WarriorJumpAttack::JumpToAttackTarget()
{
    FVector OwnerWarriorEnemyLocation = OwnerWarriorEnemy->GetActorLocation();
    AActor* AttackTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if(!AttackTarget){
        UE_LOG(LogTemp, Warning, TEXT("Get AttackTarget is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    FVector LaunchVelocity;
    float GravityZ = -980.0f; // 기본 중력
    float ArcHeight = 500.0f; // 포물선의 최대 높이
    bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        this,                 // 월드 컨텍스트
        LaunchVelocity,       // 결과 속도
        OwnerWarriorEnemyLocation,        // 시작 위치
        AttackTarget->GetActorLocation(),       // 목표 위치
        GravityZ,              // 중력 값
        ArcHeight              // 포물선 높이
    );

    if (!bSuccess) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to calculate projectile velocity (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    // 성공적으로 계산된 속도를 사용하여 액터를 발사
    OwnerWarriorEnemy->LaunchCharacter(LaunchVelocity, true, true);
}

// FVector UBTTask_WarriorJumpAttack::CalculateFutureTargetLocation(AActor* Target, float Time)
// {

// }
