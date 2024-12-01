// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;
	
	// Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	// Capsule->SetupAttachment(RootComponent);

	// ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Nigara Effect"));
	// ItemEffect -> SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
}

float AItem::TransformSin()
{
	return Amplitude * FMath::Sin(RunningTime*TimeConstant);
}

float AItem::TransformCos()
{
	return Amplitude * FMath::Cos(RunningTime*TimeConstant);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;

	//if(ItemState == EItemState::EIS_Hovering){
	AddActorWorldOffset(FVector(0.f, 0.f, TransformSin()));
	FQuat ActorRotator = FQuat(FRotator(0, 360*DeltaTime, 0));
	AddActorLocalRotation(ActorRotator);
	//}
}

void AItem::PlayPickupSound()
{
	if(PickupSound){
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}
