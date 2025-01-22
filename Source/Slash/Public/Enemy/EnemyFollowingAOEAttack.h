// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEAttack.h"
#include "EnemyFollowingAOEAttack.generated.h"

class AEnemy;

UCLASS()
class SLASH_API AEnemyFollowingAOEAttack : public AEnemyAOEAttack
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	AEnemy* OwnerEnemy;
};
