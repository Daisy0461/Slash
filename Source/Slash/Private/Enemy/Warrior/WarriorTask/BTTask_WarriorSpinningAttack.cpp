// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorSpinningAttack.h"
#include "Enemy/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_WarriorSpinningAttack::UBTTask_WarriorSpinningAttack()
{
    bNotifyTick = true; // TickTask 활성화
}

EBTNodeResult::Type UBTTask_WarriorSpinningAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(!ControllingPawn){
        UE_LOG(LogTemp, Warning, TEXT("OwnerComp's ControllingPawn cast Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogTemp, Warning, TEXT("AIController is null (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    BlackboardComp = OwnerComp.GetBlackboardComponent();
    if(!BlackboardComp){
        UE_LOG(LogTemp, Warning, TEXT("Get Blackboard Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    AttackTarget = Cast<AActor>(BlackboardComp->GetValueAsObject(AttackTargetKey.SelectedKeyName));
    if(!AttackTarget){
        UE_LOG(LogTemp, Warning, TEXT("AttackTarget is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }else{
        //UE_LOG(LogTemp, Display, TEXT("AttackTarget is %s"), *AttackTarget->GetName());
    }

    OwnerEnemy =  Cast<AEnemy>(ControllingPawn);
    if(!OwnerEnemy){
        UE_LOG(LogTemp, Warning, TEXT("Cast to Enemy Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return EBTNodeResult::Failed;
    }

    FAIEnemyAttackFinished OnAttackFinished;
    OnAttackFinished.BindLambda(
        [&]()
        {
            if(BlackboardComp && BlackboardComp->GetValueAsBool("IsAttacking")){
                BlackboardComp->SetValueAsBool("IsAttacking", false);
            }
            RotateToTarget();
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OriginRotation = OwnerEnemy->GetMesh()->GetRelativeRotation();
    //UE_LOG(LogTemp, Display, TEXT("OriginRotation: %s"), *OriginRotation.ToString());
    OwnerEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    ChaseToTarget();
    //OwnerEnemy->LongRangeAttack_Spinning();
    OwnerEnemy->EnemyAOEAttack(EEnemyAOEAttackEnum::EEAA_SpinningAttack);
    return EBTNodeResult::InProgress; 
}

void UBTTask_WarriorSpinningAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    //UE_LOG(LogTemp, Display, TEXT("TickTask"));
    ChaseToTarget();
}

void UBTTask_WarriorSpinningAttack::ChaseToTarget()
{
    //UE_LOG(LogTemp, Display, TEXT("InChaseToTarget"));
	if(BlackboardComp && AIController && AttackTarget){
        //UE_LOG(LogTemp, Display, TEXT("MoveToTarget"));
        AIController->MoveToActor(AttackTarget, 150.f, true, true, true, nullptr, true);

        // float DistanceToTarget = FVector::Dist(AIController->GetPawn()->GetActorLocation(), AttackTarget->GetActorLocation());
        // UE_LOG(LogTemp, Display, TEXT("Distance to Target: %f"), DistanceToTarget);
	}else{
        UE_LOG(LogTemp, Warning, TEXT("ChaseToTarget Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void UBTTask_WarriorSpinningAttack::RotateToTarget()
{
    if (OwnerEnemy)
    {
        OwnerEnemy->GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
    }else{
        UE_LOG(LogTemp, Warning, TEXT("RotateToTarget Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

FString UBTTask_WarriorSpinningAttack::GetStaticDescription() const
{
    Super::GetStaticDescription();
    FString Description = FString::Printf(TEXT("AttackTargetKey: %s"), *AttackTargetKey.SelectedKeyName.ToString());
    return Description;
}