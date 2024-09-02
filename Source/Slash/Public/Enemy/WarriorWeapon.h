// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WarriorWeapon.generated.h"

class AWeapon;
class AShield;
class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UWarriorWeapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWarriorWeapon();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AEnemy* WeaponOwner;
	
	UPROPERTY(EditAnywhere, Category = "Warrior Equip")
	TSubclassOf<class AWeapon> WarriorWeapon;
	UPROPERTY(EditAnywhere, Category = "Warrior Equip")
	TSubclassOf<class AShield> WarriorShield;
		
};
