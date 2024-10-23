// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

class UBoxComponent;
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

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* ArrowBox;
	UPROPERTY(EditAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere, Category = "Movememt")
	float MinSpeed = 1000.f;
	float MaxSpeed = 7000.f;
	float MinGravity = 0.0f;
	float MaxGravity = 0.5f;
	float ArrowSpeed = 4000.f;
	
	bool isFired = false;
};