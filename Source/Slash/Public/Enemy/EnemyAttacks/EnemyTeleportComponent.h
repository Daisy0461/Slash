// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"
#include "EnemyTeleportComponent.generated.h"

class AEnemy;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyTeleportComponent();
	UFUNCTION(BlueprintCallable)
	virtual void EnemyTeleportFunction(EEnemyTeleportEnum TeleportFunctionType);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	AEnemy* OwnerEnemy;

	//TeleportFunction
	virtual void StartTeleport();
	virtual void EndTeleport();
	virtual void ActivateTeleportNiagara();
	virtual void DeactivateTeleportNiagara();
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
	float TeleportAcceptanceRadius = 20.f;
	//Teleport Fade In
	void TeleportFadeIn();
    UPROPERTY(VisibleAnywhere, Category = "Materials")
    TArray<class UMaterialInstanceDynamic*> DynamicMaterials;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleport")
    UNiagaraSystem* NiagaraSystemAsset;
	UPROPERTY(EditAnywhere, Category = "Teleport")
	UNiagaraComponent* TeleportNiagaraComp;

	
    
	//Fade
	bool isFading = false;
	float FadeDuration = 1.3f;		//최소 3.0f는 유지해야함.
	float FadeElapsedTime = 0.0f;
	float CurrentOpacity = 0.0f;
	float TargetOpacity = 1.0f;
};
