// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageEnemy.h"

AMageEnemy::AMageEnemy()
{
    
}

void AMageEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
}

