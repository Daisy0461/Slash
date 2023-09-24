// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Health.h"
#include "Character/VikingCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "NiagaraFunctionLibrary.h"

void AHealth::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    AVikingCharacter* VikingCharacter = Cast<AVikingCharacter>(OtherActor);
	if(VikingCharacter){
		UAttributeComponent* VikingAttribute = VikingCharacter->GetAttribute();
        PlayPickupSound();
		if(VikingAttribute){
			VikingAttribute->Heal(HealAmount);
			VikingCharacter->SetHUDHealth();
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				HealEffect,
				GetActorLocation()
			);
		}
		Destroy();
	}
}