// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	RootComponent = ItemMesh;
	

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	world = GetWorld();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunningTime += DeltaTime;

	FQuat ActorRotator = FQuat(FRotator(0, 360*DeltaTime, 0));
	AddActorLocalRotation(ActorRotator);
}

