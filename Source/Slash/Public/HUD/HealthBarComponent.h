// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);

	UPROPERTY(EditDefaultsOnly)	//UPROPETY가 Garbage Data를 가지지 않게 하는 역할도 있다. 즉 nullptr로 만들어준다.
	class UHealthBar* HealthBarWidget;
};
