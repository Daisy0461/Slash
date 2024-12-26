// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAI/Service/BTS_UpdateDistanceToAttackTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"


void UBTS_UpdateDistanceToAttackTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
    if(!AIPawn || !AttackTarget){
        UE_LOG(LogTemp, Warning, TEXT("Your message"));
    }

    FVector AIPawnLocation = AIPawn->GetActorLocation();
    FVector AttackTargetLocation = AttackTarget->GetActorLocation();
    
    float Distance = FVector::Dist(AIPawnLocation, AttackTargetLocation);
    BlackboardComp->SetValueAsFloat(DistanceToAttackTargetKey.SelectedKeyName, Distance);
}