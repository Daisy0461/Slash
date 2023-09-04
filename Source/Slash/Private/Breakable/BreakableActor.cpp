// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Engine/World.h"
#include "Item/Treasure.h"
#include "Components/CapsuleComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

// Sets default values
ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);

	Capsule= CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}
void ABreakableActor::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	if(bBroken) return;

	bBroken = true;
	UWorld* World = GetWorld();
	if(World && TreasureClass){
		SpawnTreasure(FChaosBreakEvent());
	}
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	//아래의 코드로 Hit에 의해서 부서지지 않았어도 Treasure를 Spawn하게 하였다.
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::SpawnTreasure);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//
void ABreakableActor::SpawnTreasure(const FChaosBreakEvent& BreakEvent){
	//BP를 Spawn하는 방법임. UClass로 지정을 해준 다음 아래와 같이 Spawn해주면 됌
	//UClass를 TSubclassOf<>로 바꿨는데 그 이유는 TSubclassOf를 해주면 해당 class만 선택할 수 있기 때문에 실수를 줄일 수 있기 때문이다.
	const FVector Location = GetActorLocation() + FVector(0.0f, 0.0f, 75.f);
	GetWorld()->SpawnActor<ATreasure>(TreasureClass, Location, GetActorRotation());
}