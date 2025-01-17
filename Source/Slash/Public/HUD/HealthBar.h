// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
    //ProgressBar라는 것이 WBP에 존재한다. 아래의 C++ Healbar와 WBP의 HealthBar를 Bind하기 위해서는 WBP의 ProgressBar의 이름과
	//아래의 변수의 이름이 같아야지만 Bind된다. 꼭 같아야 Bind된다는 것을 잊으면 안된다.
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	void SetHealthBar(float Percent);
};
