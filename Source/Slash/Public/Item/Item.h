// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UNiagaraComponent;
class USoundBase;

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// UPROPERTY(EditDefaultsOnly, Category="Effects")
	// UNiagaraComponent* ItemEffect;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UWorld* world;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

private: 
	float RunningTime; 

	UFUNCTION(BlueprintPure)
	float TransformSin();

	UFUNCTION(BlueprintPure)
	float TransformCos();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PickupSound;
	void PlayPickupSound();
};
