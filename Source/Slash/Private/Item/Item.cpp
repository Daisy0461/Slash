// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "DrawDebugHelpers.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	float ActorMove = 50.f;		

	AddActorWorldOffset(FVector(ActorMove*DeltaTime, 0.f, 0.f));		//1초에 50.f만큼 동일하게 이동한다.
	
	if(world){
		DrawDebugSphere(world, GetActorLocation(), 30.f, 25, FColor::Blue, false, -1);
	}
}

