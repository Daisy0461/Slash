// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WarlordWeapon.generated.h"

class AWeapon;
class AEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UWarlordWeapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWarlordWeapon();
	virtual void SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	virtual void SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	FORCEINLINE AWeapon* GetWeapon() {return Sword;};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Warlord Equip")
	TSubclassOf<class AWeapon> WarlordWeapon;

private:
	UFUNCTION()
	void DestoryWeapons();
	float DestoryTime = 5.f;
	AWeapon* Sword;
	ACharacter* WarlordCharactor;
	APawn* WarlordPawn;
		
};
