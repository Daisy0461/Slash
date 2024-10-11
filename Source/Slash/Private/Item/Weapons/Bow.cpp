// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/Bow.h"

ABow::ABow()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABow::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}
