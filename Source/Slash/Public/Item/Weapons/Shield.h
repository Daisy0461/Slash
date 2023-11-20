// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Shield.generated.h"
/**
 * 
 */
class UBoxComponent;
class UParticleSyustem;
UCLASS()

class SLASH_API AShield : public AItem
{
	GENERATED_BODY()

public:
	AShield();
	void Equip(USceneComponent* InParent, FName InSocketName);
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void SpawnShieldParticle();

protected:
	//UFUNCTION() override할 때는 UFUNCTION()을 지워줘야한다. 아니면 에러가 발생한다.
	//오른쪽 끝을 보면 override가 있다. 이것은 컴파일러가 override임을 알게 하고 Parent Class로 가서 같은 이름의 virtual이 있는지 확인한다.
	//없다면 에러를 발생시킨다. 또한 다른 프로그래머에게 override한 함수임을 알린다.
	virtual void CapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void CapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UBoxComponent* ShieldBox;
	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitParticles;
};