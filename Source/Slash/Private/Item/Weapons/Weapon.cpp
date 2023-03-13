// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapons/Weapon.h"
#include "Charater/ParagonCharacter.h"

void AWeapon::SphereOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::SphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    AParagonCharacter* ParagonCharacter = Cast<AParagonCharacter>(OtherActor);
    if(ParagonCharacter)
    {
        ParagonCharacter->EquipOverlap();       //Overlap함을 알림.
        FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
        ItemMesh->AttachToComponent(ParagonCharacter->GetMesh(), 
                                    TransformRules, FName("RightHandSocket"));
    }
}

void AWeapon::SphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::SphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}