// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/Weapon.h"
#include "ThrowWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AThrowWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AThrowWeapon();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
protected:

private:
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* WeaponRotatePivot;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* ThrowWeaponMesh;

	FTimerHandle DestroyThrowWeaponTimeHandle;
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* ThrowWeaponHitEffect;
	UPROPERTY(EditAnywhere, Category = "ThrowWeapon")
	float ThrowWeaponSpeed = 700.f;
	UPROPERTY(EditAnywhere, Category = "ThrowWeapon")
	float ThrowWeaponRotateSpeed = 700.f;
	UPROPERTY(EditAnywhere, Category = "ThrowWeapon")
	float ThrowWeaponDuration = 3.f;

	void SpawnHitParticle();
	void DestroyThrowWeapon();
};
