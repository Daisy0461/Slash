// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"

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
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString a = FString(TEXT("asdfasdf"));
	float b = 0.1f;
	
	UE_LOG(LogTemp, Display, TEXT("Your message: %s,  %f"), *a, b);		//FString이 *을 사용하는 이유는 char 배열의 주소를 가져오기 때문이다.
}

