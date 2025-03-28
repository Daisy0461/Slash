// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Treasure.generated.h"


UCLASS()
class SLASH_API ATreasure : public AItem
{
	GENERATED_BODY()
public:
	FORCEINLINE int32 GetTreasure() {return Treasure;};

protected:
	//virtual void CapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UPROPERTY(EditAnywhere, Category="Treasure Properties")
	int32 Treasure;

};
