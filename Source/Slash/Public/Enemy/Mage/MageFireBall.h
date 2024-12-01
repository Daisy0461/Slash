// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "MageFireBall.generated.h"

class UBoxComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class SLASH_API AMageFireBall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMageFireBall();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
    void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

	//FireBall
	FTimerHandle DestroyFireBallTimeHandle;
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystemComponent* FireBallEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* FireBallHitEffect;
	UPROPERTY(EditAnywhere, Category = "FireBall")
	float Damage = 10.f;
	UPROPERTY(EditAnywhere, Category = "FireBall")
	float FireBallSpeed = 500.f;
	UPROPERTY(EditAnywhere, Category = "FireBall")
	float FireBallDuration = 3.f;

	void HitTrace(TArray<FHitResult>& HitResults);
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceEnd;
	
	void SpawnHitParticle();
	void DestroyFireBall();

};
