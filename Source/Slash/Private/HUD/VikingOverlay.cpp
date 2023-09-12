// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VikingOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UVikingOverlay::SetHealthBarPercent(float Percent)
{
    if(HealthProgressBar){
        HealthProgressBar->SetPercent(Percent);
    }
}
void UVikingOverlay::SetStaminaBarPercent(float Percent)
{
    if(StaminaProgressBar){
        StaminaProgressBar->SetPercent(Percent);
    }
}
void UVikingOverlay::SetSouls(int32 Souls)
{
    if(SoulsText){
        const FString String = FString::Printf(TEXT("%d"), Souls);
        const FText Text = FText::FromString(String);
        SoulsText->SetText(Text);
    }
}
