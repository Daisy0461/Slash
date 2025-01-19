// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Service/BTS_RotateToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBTS_RotateToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    if(!AIController){
        UE_LOG(LogTemp, Warning, TEXT("AAIController cant find (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if(!BlackboardComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("BlackboardComp cant find (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }
    APawn* AIPawn = AIController->GetPawn();
    AActor* AttackTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    // bool bIsAttacking = BlackboardComp->GetValueAsBool(AttackingKey.SelectedKeyName);

    // if(bIsAttacking){
    //     return;
    // }

    if(!AIPawn || !AttackTarget){
        UE_LOG(LogTemp, Warning, TEXT("AIPawn or AttackTarget is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    FVector AIPawnLocation = AIPawn->GetActorLocation();
    FRotator CurrentRotation = AIPawn->GetActorRotation();
    FVector AttackTargetLocation = AttackTarget->GetActorLocation();

    FVector Direction = (AttackTargetLocation - AIPawnLocation).GetSafeNormal();
    FRotator TargetRotation = Direction.Rotation();

    FRotator SmoothRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaSeconds, DeltaSeconds);

    AIPawn->SetActorRotation(SmoothRotation);
    UE_LOG(LogTemp, Display, TEXT("Rotate to AttackTarget"));
}

FString UBTS_RotateToTarget::GetStaticDescription() const
{
    return FString::Printf(TEXT("Rotate to %s"), *AttackTargetKey.SelectedKeyName.ToString());
}