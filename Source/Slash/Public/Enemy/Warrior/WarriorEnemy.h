// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "WarriorEnemy.generated.h"

class UWarriorWeapon;

UCLASS()
class SLASH_API AWarriorEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	AWarriorEnemy();


private:
	UPROPERTY(VisibleAnywhere)
	UWarriorWeapon* WarriorWeapon;

	UFUNCTION(BlueprintCallable)
	virtual void Attack();

	
};