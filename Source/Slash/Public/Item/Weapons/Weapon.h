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
class UParticleSyustem;

UCLASS()

class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	FORCEINLINE UBoxComponent* GetWeaponBox()  const {return WeaponBox;}
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void SpawnWeaponParticle();
	void OverlappedActorClear();
	TArray<AActor*> IgnoreActors;
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void OnParryBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintImplementableEvent)		//C++에서 Body 구현 불가능, BP에서 만든다.
	void CreateFields(const FVector& FieldLoaction);
	

private:
	//void HitTrace(FHitResult& BoxHit);
	void HitTrace(TArray<FHitResult>& HitResults);
	void HitInterface(const FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* ParryBox;
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

	//HitStop
	void StartHitStop(const float DamageAmount);
	AActor* AttackActor;
	AActor* HittedActor;
	void EndHitStop();
	FTimerHandle HitStopTimerHandle;
	UPROPERTY(EditAnywhere, Category = "Hit Stop")
	float HitStopModifier = 0.01f;		//damage에 따라 다른 시간을 적용하기 위해 사용

	//ShieldParticle
	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitParticles;

	TSet<AActor*> OverlappedActors;

	
	//test
	int32 TestHitCount = 0;
};