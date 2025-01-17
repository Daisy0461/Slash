// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VikingOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "HUD/HealthBar.h"

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
void UVikingOverlay::SetTreasures(int32 Treasures)
{
    if(TreasureText){
        const FString String = FString::Printf(TEXT("%d"), Treasures);
        const FText Text = FText::FromString(String);
        TreasureText->SetText(Text);
    }
}

void UVikingOverlay::SetBowIndicatorVisible(bool isVisible)
{
    BP_BowIndicator->SetVisibility(isVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UVikingOverlay::AddBossHealthBarAtVertticalBox(UHealthBar* BossHealthBar)
{
    if (TopVerticalBox && BossHealthBar) // TopVerticalBox는 바인딩된 VerticalBox
    {
        // VerticalBox에 BossHealthBar 추가
        UVerticalBoxSlot* NewSlot = TopVerticalBox->AddChildToVerticalBox(BossHealthBar);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TopVerticalBox or BossHealthBar is invalid!"));
    }
}
