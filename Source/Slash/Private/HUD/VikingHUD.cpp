// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/VikingHUD.h"
#include "HUD/VikingOverlay.h"

void AVikingHUD::BeginPlay()
{
    Super::BeginPlay();
    UWorld* World = GetWorld();
    if(World){
        APlayerController* Controller = World->GetFirstPlayerController();
        if(Controller && VikingOverlayClass)
        {
            VikingOverlay = CreateWidget<UVikingOverlay>(World->GetFirstPlayerController(), VikingOverlayClass); 
            VikingOverlay->AddToViewport();
        }
    }
    
}