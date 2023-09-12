// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "VikingHUD.generated.h"

class UVikingOverlay;

UCLASS()
class SLASH_API AVikingHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE UVikingOverlay* GetVikingOverlay() const {return VikingOverlay;}

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Viking")
	TSubclassOf<UVikingOverlay> VikingOverlayClass; 

	UPROPERTY()
	UVikingOverlay* VikingOverlay;
};
