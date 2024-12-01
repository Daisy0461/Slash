// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapons/Weapon.h"
#include "MageIceBall.generated.h"

UCLASS()
class SLASH_API AMageIceBall : public AWeapon
{
	GENERATED_BODY()
public:
	AMageIceBall();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

private:
	//Ball
	FTimerHandle DestroyFireBallTimeHandle;
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystemComponent* FireBallEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* FireBallHitEffect;
	UPROPERTY(EditAnywhere, Category = "FireBall")
	float FireBallSpeed = 500.f;
	UPROPERTY(EditAnywhere, Category = "FireBall")
	float FireBallDuration = 3.f;

	void SpawnHitParticle();
	void DestroyFireBall();
};
