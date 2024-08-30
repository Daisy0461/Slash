// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"
/**
 * 
 */
class UBoxComponent;
class USceneComponent;
class AVikingCharacter;

UCLASS()

class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	FORCEINLINE UBoxComponent* GetWeaponBox()  const {return WeaponBox;}
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;
	virtual void CapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void CapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)		//C++에서 Body 구현 불가능, BP에서 만든다.
	void CreateFields(const FVector& FieldLoaction);

private:
	void HitTrace(FHitResult& BoxHit);
	void HitInterface(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceStart;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceEnd;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector WeaponBoxTraceExtend = FVector(45.f, 15.f, 45.f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = true;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 10.f;
};