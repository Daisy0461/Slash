// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "MageEnemy.generated.h"

class USceneComponent; 

UCLASS()
class SLASH_API AMageEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AMageEnemy();
	virtual void BeginPlay() override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed) override;
	virtual void AttackByAI() override;

	void StartTeleport();
	void EndTeleport();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* FireBallMontage;
	UPROPERTY(EditAnywhere, Category = "Attack")
	USceneComponent* FirePosition;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> FireBall;
	UFUNCTION(BlueprintCallable)
	void SpawnFireBall();

private:	
	//Teleport
	void HideMesh(bool doHide);
	void SpawnTeleportEffets(bool doSpawn);
	void IgnoreCollision(bool doIgnore);
	UPROPERTY(EditAnywhere, Category = "Teleport")
    UParticleSystem* TeleportEffect;
	UPROPERTY(EditAnywhere, Category = "Teleport")
    UParticleSystem* TeleportEffectTrail;
	UPROPERTY()
	UParticleSystemComponent* TeleportBodyEffectComp;
	UPROPERTY()
	UParticleSystemComponent* TeleportEffectTrailComp;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportSpeed = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	float TeleportAcceptanceRadius = 20.f;
};
