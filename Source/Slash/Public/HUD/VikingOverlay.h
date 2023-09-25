// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VikingOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class SLASH_API UVikingOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetTreasures(int32 Treasures);

private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar; 

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TreasureText;
};
