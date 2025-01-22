// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFollowingAOEAttack.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Enemy/Enemy.h"

AEnemyFollowingAOEAttack::AEnemyFollowingAOEAttack()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyFollowingAOEAttack::BeginPlay()
{
    Super::BeginPlay();
    
    if(GetOwner()){
        OwnerEnemy = Cast<AEnemy>(GetOwner());
        if(OwnerEnemy){
            BaseEnemyAIController = OwnerEnemy->GetBaseEnemyAIController();
            if(BaseEnemyAIController){
                AttackTarget = BaseEnemyAIController->GetAttackTargetActor();
            }else{
                UE_LOG(LogTemp, Warning, TEXT("Get BaseEnemyAIC Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
            }
        }else{
            UE_LOG(LogTemp, Warning, TEXT("Enemy Cast is Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
        }
    }else{
        UE_LOG(LogTemp, Warning, TEXT("GetOwner is Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void AEnemyFollowingAOEAttack::Tick(float DeltaTime)
{
    if(!AttackTarget) return;

    FVector CurrentLocation = GetActorLocation();
    FVector TargetLocation = OwnerEnemy->GetGroundLocation(AttackTarget);

    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, FollowingSpeed);

    SetActorLocation(NewLocation);
}
