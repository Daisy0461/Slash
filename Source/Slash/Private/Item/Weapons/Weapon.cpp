// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapons/Weapon.h"
#include "Character/VikingCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox -> SetupAttachment(GetRootComponent());
}

void AWeapon::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::CapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::CapsuleEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::CapsuleEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    //이 Item(Weapon)의 Mesh를 Parents에게 붙이고 FName으로 들어온 Socket에 붙이겠다는 의미이다.
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
    ItemState = EItemState::EIS_Equipped;
    // if(Capsule){
    //     //이후에 다시 Overlapp되서 혼동이 발생하는걸 막는 것이다.Collision Check박스를 Ignore로 바꾼다고 생각하면 된다.
    //     Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // }
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}