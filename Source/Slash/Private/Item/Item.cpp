// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Charater/ParagonCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	RootComponent = ItemMesh;
	
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//OnComponentBeginOverlap이 Delegate이름임. AddDynamic은 Delegate에 추가하는 것이고 this로 추가된 첫번째 파라미터는 두번쨰 파라미터인 콜백함수가 있는 개체를 넣으면 되서 this를 넣은 것이다. 다른 곳에 있다면 다른곳의 포인터를 넣으면 된다.
	//생성자에서는 모든 Component들이 초기화가 안됐을 수 있기 때문에 Begin에서 Delegate에 추가한다.
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::SphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::SphereEndOverlap);
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

	//FQuat ActorRotator = FQuat(FRotator(0, 360*DeltaTime, 0));
	//AddActorLocalRotation(ActorRotator);
}
void AItem::SphereOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AParagonCharacter* ParagonCharacter = Cast<AParagonCharacter>(OtherActor);
	if(ParagonCharacter){
		ParagonCharacter->SetOverlappingItem(this);
	}
}

void AItem::SphereEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	AParagonCharacter* ParagonCharacter = Cast<AParagonCharacter>(OtherActor);
	if(ParagonCharacter){
		ParagonCharacter->SetOverlappingItem(nullptr);
	}
}
