// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "WarlordEnemy.generated.h"

class UWarriorWeapon;
class UWarlordWeapon;
class AWeapon;
class AWarlordEnemyAIController;

UCLASS()
class SLASH_API AWarlordEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AWarlordEnemy();
	

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void SetWarlordWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	void SetWarlordParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWarlordWeapon();

private:
	UPROPERTY(VisibleAnywhere)
	UWarlordWeapon* WarlordWeapon;
	AWarlordEnemyAIController* WarlordEnemyAIController;
	
};
