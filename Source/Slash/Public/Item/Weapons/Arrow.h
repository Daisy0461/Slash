// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class UBoxComponent;
class USceneComponent;
class UNiagaraSystem;
class UParticleSystem;
class UProjectileMovementComponent;

UCLASS()
class SLASH_API AArrow : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void SetArrowFire(FVector Direction, float Strength);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	FVector GetArrowLocation();
	FORCEINLINE bool GetIsFired() const {return isFired; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnArrowBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* ArrowBox;
	UPROPERTY(EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* ArrowTraceStart;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* ArrowTraceEnd;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere, Category = "Movememt")
	float MinSpeed = 1000.f;
	float MaxSpeed = 7000.f;
	float MinGravity = 0.0f;
	float MaxGravity = 0.5f;
	float ArrowSpeed = 4000.f;
	
	//Fire
	bool isFired = false;
	UPROPERTY(EditAnywhere, Category = "Trace")
	FVector ArrowTraceExtend = FVector(10.0f, 10.0f, 10.0f); // Box 크기 설정
	FTimerHandle ArrowTimerHandle;
	UPROPERTY(EditAnywhere)
	USceneComponent* SceneComponent;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* NiagaraSystem;
	UFUNCTION(Category="Effects")
    void SpawnAttachedNiagaraSystem();
	void SetShotProjectileMovemet(FVector Direction, float Strength);
	void SetArrowCollision();
	void DestroyArrow();
	UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* ArrowImpactParticle;

	void HitTrace(TArray<FHitResult>& HitResults);

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowBoxDebug = false; // 디버그 트레이스 시각화
};