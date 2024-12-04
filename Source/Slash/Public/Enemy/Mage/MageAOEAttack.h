// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyAOEBase.h"
#include "Interfaces/HitInterface.h"
#include "MageAOEAttack.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API AMageAOEAttack : public AEnemyAOEBase
{
	GENERATED_BODY()
public:

protected:
	virtual void BeginPlay() override;
	virtual void OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;	
	UFUNCTION()
	virtual void OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void DamageActor(IHitInterface* HitInterface, AActor* DamagedActor);		//UFUNCTION이 없으면 IHitIntferface만 해도 된다.
	FTimerHandle AOEDamageTimer;
	FTimerHandle AOEDestroyTimer;
	UPROPERTY(EditAnywhere)
	float DurationTime = 3.f;

};
