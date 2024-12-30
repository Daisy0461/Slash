// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorJumpAttack.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"

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
    JumpToAttackTarget();
    OwnerWarriorEnemy->LongRangeAttack_Jump();
    return EBTNodeResult::InProgress; 
}

void UBTTask_WarriorJumpAttack::JumpToAttackTarget()
{
    FVector OwnerWarriorEnemyLocation = OwnerWarriorEnemy->GetActorLocation();
    AttackTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if (!AttackTarget) {
        UE_LOG(LogTemp, Warning, TEXT("Get AttackTarget is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    FVector JumpLocation = CalculateFutureTargetLocation(AttackTarget, 1.f);
    FVector LaunchVelocity;
    float GravityZ = -850.0f;
    float Distance = FVector::Dist(OwnerWarriorEnemyLocation, JumpLocation);
    float ZDifference = JumpLocation.Z - OwnerWarriorEnemyLocation.Z;
    float ArcParam = FMath::Clamp((Distance + ZDifference) / 1000.f, 0.0f, 1.0f);

    UWorld* World = GetWorld();
    if (!World) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to Get World (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    FHitResult HitResult;
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        OwnerWarriorEnemyLocation,
        JumpLocation,
        ECC_Visibility
    );

    if (bHit) {
        UE_LOG(LogTemp, Warning, TEXT("Path obstructed by: %s"), *HitResult.GetActor()->GetName());
        // 충돌이 감지되면 목표 위치를 약간 위로 보정
        JumpLocation.Z += 100.f;
    }

    bool bSuccess = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
        World,
        LaunchVelocity,
        OwnerWarriorEnemyLocation,
        JumpLocation,
        GravityZ,
        ArcParam
    );

    if (!bSuccess) {
        UE_LOG(LogTemp, Warning, TEXT("Failed to calculate projectile velocity"));
        UE_LOG(LogTemp, Warning, TEXT("Distance: %f, Z Difference: %f, ArcHeight: %f"), Distance, ZDifference, ArcParam);
        return;
    }

    // UE_LOG(LogTemp, Warning, TEXT("Distance: %f, Z Difference: %f, ArcHeight: %f"), Distance, ZDifference, ArcParam);
    // UE_LOG(LogTemp, Warning, TEXT("Calculated Launch Velocity: %s"), *LaunchVelocity.ToString());
    OwnerWarriorEnemy->LaunchCharacter(LaunchVelocity, true, true);
}

FVector UBTTask_WarriorJumpAttack::CalculateFutureTargetLocation(AActor* Target, float Time)
{
    if(!AttackTarget){
        UE_LOG(LogTemp, Warning, TEXT("AttackTarget is nullptr in calculate (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return FVector(0, 0, 0);
    }
    FVector TargetVelocity = AttackTarget->GetVelocity();
    if (TargetVelocity.IsNearlyZero()) {
        //UE_LOG(LogTemp, Warning, TEXT("Target is stationary (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return Target->GetActorLocation();
    }
    
    FVector IgnoreZVaule = TargetVelocity * FVector(1.f, 1.f, 0.f);
    FVector CalculateLoation = IgnoreZVaule * Time;
    FVector FutureLocation = CalculateLoation + AttackTarget->GetActorLocation();

    return FutureLocation;
}