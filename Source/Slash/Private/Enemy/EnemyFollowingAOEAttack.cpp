// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyFollowingAOEAttack.h"
#include "Enemy/Enemy.h"

void AEnemyFollowingAOEAttack::BeginPlay()
{
    Super::BeginPlay();
    
    if(GetOwner()){
        OwnerEnemy = Cast<AEnemy>(GetOwner());
    }else{
        UE_LOG(LogTemp, Warning, TEXT("GetOwner is Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

