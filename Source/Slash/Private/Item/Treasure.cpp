// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Treasure.h"
#include "Character/VikingCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    AVikingCharacter* VikingCharacter = Cast<AVikingCharacter>(OtherActor);
	if(VikingCharacter){
		VikingCharacter->AddTreasure(this);
        PlayPickupSound();
		Destroy();
	}
}
