// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEBase.h"
#include "Interfaces/HitInterface.h"
#include "EnemyAOEAttack.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AEnemyAOEAttack : public AEnemyAOEBase
{
	GENERATED_BODY()
public:
	AEnemyAOEAttack();
protected:
	virtual void BeginPlay() override;
	virtual void OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;	
	UFUNCTION()
	virtual void OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void DamageActor(IHitInterface* HitInterface, AActor* DamagedActor);
	void ActiveAOEDamage();
	void DestroyAOE();

	FTimerHandle AOEDamageTimer;
	FTimerHandle AOEDestroyTimer;
	UPROPERTY(EditAnywhere)
	bool bIsPlayHitReaction = false;
	UPROPERTY(EditAnywhere)
    UParticleSystemComponent* CautionAOEEffect;
	UPROPERTY(EditAnywhere)
	bool bIsImmediateDamage = false;
	UPROPERTY(EditAnywhere)
	float CautionDuration = 2.f;
	UPROPERTY(EditAnywhere)
	float DamageDurationTime = 3.f;
};
