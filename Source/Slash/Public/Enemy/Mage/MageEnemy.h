// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "MageEnemy.generated.h"

class USceneComponent; 
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class UNiagaraComponent;
class UNiagaraSystem;
class AEnemyAOEAttack;
class AEnemyAreaHeal;

UCLASS()
class SLASH_API AMageEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AMageEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	void SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed) override;
	virtual void FireBallAttack(); 
	virtual void BarrageAttack();
	virtual void MageAOEAttack();
	virtual void MageHealing(); 

	//virtual void ShortRangeAttack() override;

	
	void StartTeleport();
	void EndTeleport();
	void ActivateTeleportNiagara();
	void DeactivateTeleportNiagara();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* FireBallMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* BarrageMontage;
	UPROPERTY(EditAnywhere, Category = "Attack")
	USceneComponent* FirePosition;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> FireBall;
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<AActor> BarrageBall;
	UFUNCTION(BlueprintCallable)
	void SpawnFireBall(bool bIsBarrageBall);
	UFUNCTION(BlueprintCallable)
	void SpawnAOE();

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
	//Teleport Fade In
	void TeleportFadeIn();
    UPROPERTY(VisibleAnywhere, Category = "Materials")
    TArray<class UMaterialInstanceDynamic*> DynamicMaterials;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	UNiagaraComponent* TeleportNiagaraComp;

	//Healing
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	void DestroyHealingArea();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* MageHealingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Healing")
	TSubclassOf<AEnemyAreaHeal> HealingAreaClass;
	AEnemyAreaHeal* HealingArea;

	//MageAOE
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* MageAOEAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Healing")
	TSubclassOf<AEnemyAOEAttack> MageAOEClass;

	//Fade
	bool isFading = false;
	float FadeDuration = 1.3f;		//최소 3.0f는 유지해야함.
	float FadeElapsedTime = 0.0f;
	float CurrentOpacity = 0.0f;
	float TargetOpacity = 1.0f;
};
