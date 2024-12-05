// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyAOEBase.generated.h"

class UCapsuleComponent;
class USceneComponent;

UCLASS()
class SLASH_API AEnemyAOEBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyAOEBase();
	virtual void Tick(float DeltaTime) override; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);	
	UFUNCTION()
	virtual void DestroyAOE();
	UPROPERTY(EditAnywhere)
	float DestroyTime = 10.f;
	UPROPERTY(EditAnywhere, Category = "AOE")
	bool bDrawAOECapsule = true;
	UPROPERTY(EditAnywhere, Category = "AOE")
	bool bIsAutoActive = true;
	bool bIsIgnoreSelf = false;
	bool bIsIgnoreEnemy = false;
	
	UPROPERTY(EditAnywhere, Category = "AOE")
	float DrawTime = 2.f;
	UPROPERTY(EditAnywhere, Category = "AOE")
	float AOEDamage = 2.f;

	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, Category = "AOE")
	UCapsuleComponent* CapsuleComp;
	UPROPERTY(EditAnywhere, Category = "AOE")
    UParticleSystemComponent* AOEEffect;
};

