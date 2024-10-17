// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Components/TimelineComponent.h"
#include "Bow.generated.h"

class UCurveFloat;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SLASH_API ABow : public AItem
{
	GENERATED_BODY()
public:
	ABow();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void StartAiming();
    void StopAiming();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 8.f;
	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* BowSound;

	//Bow Camera
	UCameraComponent* CameraComponent;
	USpringArmComponent* SpringArm;
	bool isAiming = false;
	float InitialFieldOfView = 90.f;
	float AimFieldOfView = 45.f;	
	FVector InitialCameraOffset = FVector(0, 0, 150.f);
	FVector AimCameraOffset = FVector(0, 0, 180.f);

	//FTimeline BowTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* AimCurve;
	UFUNCTION()
    void TimelineUpdate(float Value);

	FTimeline AimTimeline;
};
