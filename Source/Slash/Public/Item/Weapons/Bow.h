// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Components/TimelineComponent.h"
#include "Bow.generated.h"

class AArrow;
class UCurveFloat;
class UCameraComponent;
class USpringArmComponent;
class UCameraComponent;
class USpringArmComponent;
class UAudioComponent;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAimOngoing, float, MaxDrawTime, float, DrawTime);

UCLASS()
class SLASH_API ABow : public AItem
{
	GENERATED_BODY()
public:
	ABow();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	FORCEINLINE bool GetIsSpawnArrow() const {return isSpawnArrow;};
	void StartAiming();
    void StopAiming();
	FVector GetArrowLocation();
	virtual void FireArrow(FVector Direction);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 8.f;

	//Bow Camera
	UCameraComponent* CameraComponent;
	USpringArmComponent* SpringArm;
	bool isAiming = false;
	float InitialFieldOfView = 90.f;
	float AimFieldOfView = 45.f;	
	FVector InitialCameraOffset = FVector(0, 0, 150.f);
	FVector AimCameraOffset = FVector(0, 0, 180.f);

	//FTimeline BowTimeline;
	FTimeline AimTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* AimCurve;
	UFUNCTION()
    void TimelineUpdate(float Value);
	

	//Arrow
	UPROPERTY()
	AArrow* Arrow;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AArrow> SpawnedArrow;
	virtual void SpawnArrow();
	bool isSpawnArrow = false;
	virtual void DestoryArrow();

	//Aim
	FTimerHandle AimTimerHandle;
	void IncreaseDrawTime();
	void ClearAimTimer();
	float DrawIncreaseTime = 0.05f;
	float MaxDrawTime = 1.f;
	float DrawTime;
	UPROPERTY(EditAnywhere, Category = "Sound")
	UAudioComponent* AudioComponent;			//중간에 멈춰야해서 AurioComponent로 설정
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* ShotSound;
	UPROPERTY(BlueprintAssignable)
	FOnAimOngoing OnAimOngoing;

};
