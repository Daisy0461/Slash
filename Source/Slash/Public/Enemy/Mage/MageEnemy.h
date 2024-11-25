// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "MageEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AMageEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AMageEnemy();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
};
