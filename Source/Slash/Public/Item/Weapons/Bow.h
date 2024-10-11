// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Bow.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ABow : public AItem
{
	GENERATED_BODY()
public:
	ABow();
	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName);

protected:

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 8.f;
	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* BowSound;
};
