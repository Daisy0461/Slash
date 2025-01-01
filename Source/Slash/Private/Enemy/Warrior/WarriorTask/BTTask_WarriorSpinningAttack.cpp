// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorTask/BTTask_WarriorSpinningAttack.h"
#include "Enemy/Warrior/WarriorEnemy.h"
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
        UE_LOG(LogTemp, Display, TEXT("AttackTarget is %s"), *AttackTarget->GetName());
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
            if(BlackboardComp && BlackboardComp->GetValueAsBool("IsAttacking")){
                BlackboardComp->SetValueAsBool("IsAttacking", false);
            }
            RotateToTarget();
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        }
    );

    OriginRotation = OwnerWarriorEnemy->GetMesh()->GetRelativeRotation();
    OwnerWarriorEnemy->SetAIAttackFinishDelegate(OnAttackFinished);
    BlackboardComp->SetValueAsBool("IsAttacking", true);
    ChaseToTarget();
    OwnerWarriorEnemy->LongRangeAttack_Spinning();
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
    if (AttackTarget && OwnerWarriorEnemy)
    {
        FVector TargetDirection = AttackTarget->GetActorLocation() - OwnerWarriorEnemy->GetActorLocation();
        TargetDirection.Z = 0; // 수평 회전만 고려
        TargetDirection.Normalize();

        FRotator TargetRotation = TargetDirection.Rotation();
        OwnerWarriorEnemy->GetMesh()->SetRelativeRotation(TargetRotation);
    }
    else if (OwnerWarriorEnemy)
    {
        OwnerWarriorEnemy->GetMesh()->SetRelativeRotation(OriginRotation);
    }
}

FString UBTTask_WarriorSpinningAttack::GetStaticDescription() const
{
    Super::GetStaticDescription();
    FString Description = FString::Printf(TEXT("\nAttackTargetKey: %s"), *AttackTargetKey.SelectedKeyName.ToString());
    return Description;
}