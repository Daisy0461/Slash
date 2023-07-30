// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Character/VikingCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;
	
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComponent);

	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Nigara Effect"));
	Niagara -> SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	//OnComponentBeginOverlap이 Delegate이름임. AddDynamic은 Delegate에 추가하는 것이고 this로 추가된 첫번째 파라미터는 두번쨰 파라미터인 콜백함수가 있는 개체를 넣으면 되서 this를 넣은 것이다. 다른 곳에 있다면 다른곳의 포인터를 넣으면 된다.
	//생성자에서는 모든 Component들이 초기화가 안됐을 수 있기 때문에 Begin에서 Delegate에 추가한다.
	Capsule->OnComponentBeginOverlap.AddDynamic(this, &AItem::CapsuleOverlap);
	Capsule->OnComponentEndOverlap.AddDynamic(this, &AItem::CapsuleEndOverlap);
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

	if(ItemState == EItemState::EIS_Hovering){
		AddActorWorldOffset(FVector(0.f, 0.f, TransformSin()));
		FQuat ActorRotator = FQuat(FRotator(0, 360*DeltaTime, 0));
		AddActorLocalRotation(ActorRotator);
	}
}
void AItem::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AVikingCharacter* VikingCharacter = Cast<AVikingCharacter>(OtherActor);
	if(VikingCharacter){
		VikingCharacter->SetOverlappingItem(this);		//이 Item을 Viking의 OverlappingItem으로 바꿔준다.
	}
}

void AItem::CapsuleEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	AVikingCharacter* VikingCharacter = Cast<AVikingCharacter>(OtherActor);
	if(VikingCharacter){
		VikingCharacter->SetOverlappingItem(nullptr);	//해당 Overlapping 되는 부분을 벗어났을 때 OverlappingItem을 초기화시켜준다.
	}
}
