// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/Arrow.h"
#include "Components/BoxComponent.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	// ArrowMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// ArrowMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// ArrowMesh->SetRelativeRotation(FRotator(0, 0, 90.f));
	// RootComponent = ArrowMesh;

	ArrowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Arrow Box"));
    ArrowBox -> SetupAttachment(GetRootComponent());
	ArrowBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ArrowBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    ArrowBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AArrow::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AArrow::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
}