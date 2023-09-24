// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Health.generated.h"

class UNiagaraSystem;

UCLASS()
class SLASH_API AHealth : public AItem
{
	GENERATED_BODY()
protected:
	virtual void CapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UPROPERTY(EditAnywhere, Category="Health Properties")
	float HealAmount = 10.f;

	//Effect
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* HealEffect;
};
 