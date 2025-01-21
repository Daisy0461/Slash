// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordTask/BTTask_WarlordPlatformUpAndDown.h"
#include "Enemy/Warlord/WarlordEnemy.h"
#include "AIController.h"

EBTNodeResult::Type UBTTask_WarlordPlatformUpAndDown::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
    
    APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
    if(nullptr == ControllingPawn){
        UE_LOG(LogTemp, Display, TEXT("BTTask_WarloUBTTask_WarlordPlatformUpAndDown Can't find Pawn"));
        return EBTNodeResult::Failed;
    }

    AWarlordEnemy* OwnerWarlordEnemy = Cast<AWarlordEnemy>(ControllingPawn);
    if(nullptr == OwnerWarlordEnemy){
        UE_LOG(LogTemp, Display, TEXT("BTTask_WarloUBTTask_WarlordPlatformUpAndDown Cast Failed"));
        return EBTNodeResult::Failed;
    }

    //OwnerWarlordEnemy->ShortRangeAttack();
    if(bIsPlatformUp){
        OwnerWarlordEnemy->SpawnFloatingPlatform();
    }else{
        OwnerWarlordEnemy->FloatingPlatformDown();
    }
    return EBTNodeResult::Succeeded; 
}

FString UBTTask_WarlordPlatformUpAndDown::GetStaticDescription() const
{
    return FString::Printf(TEXT("SetEnemyState: %s"), bIsPlatformUp ? TEXT("Platform Up") : TEXT("Platform Down"));
}