// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEBase.h"
#include "EnemyExplosionAttack.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AEnemyExplosionAttack : public AEnemyAOEBase
{
	GENERATED_BODY()
public:
	AEnemyExplosionAttack();

protected:
	virtual void BeginPlay();
	virtual void OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);	
	UFUNCTION()
	void OnAOEEffectFinished(UParticleSystemComponent* FinishedComponent);
	UPROPERTY(EditAnywhere)
    UParticleSystemComponent* CautionAOEEffect;
	TArray<AActor*> IgnoreActors;

	//Caution 관련
	UPROPERTY(EditAnywhere, Category = "AOE")
	bool bIsImmediateDamage = false;
	UPROPERTY(EditAnywhere, Category = "AOE")
	float CautionEffectDurtaion = 5.f;
	FTimerHandle ActiveDamageTimer;
	void ActiveAOEDamage();


};
