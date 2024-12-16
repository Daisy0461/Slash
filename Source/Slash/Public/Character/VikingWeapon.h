// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VikingWeapon.generated.h"

class ACharacter;
class AWeapon;
class ABow;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UVikingWeapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVikingWeapon();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	virtual void SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	virtual bool ChangeEquip();
	FORCEINLINE AWeapon* GetAxe() {return Axe;};
	FORCEINLINE AWeapon* GetShield() {return Shield;};
	FORCEINLINE ABow* GetBow() {return Bow; };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool AttachAxeAndShieldWeapon();
	bool AttachBowWeapon();
	
	UPROPERTY(EditAnywhere, Category = "Viking Equip")
	TSubclassOf<class AWeapon> VikingAxe;
	UPROPERTY(EditAnywhere, Category = "Viking Equip")
	TSubclassOf<class AWeapon> VikingShield;
	UPROPERTY(EditAnywhere, Category = "Viking Equip")
	TSubclassOf<class ABow> VikingBow;


private:	
	AActor* VikingActor;
	ACharacter* VikingCharacter;
	APawn* VikingPawn;

	AWeapon* Axe;
	AWeapon* Shield;
	ABow* Bow;
	bool bIsEquippingAxe = true;

		
};
