// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VikingWeapon.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Bow.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UVikingWeapon::UVikingWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVikingWeapon::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();

	if(World && (VikingAxe ||  VikingShield || VikingBow)){
		Axe = World->SpawnActor<AWeapon>(VikingAxe);
		Shield = World->SpawnActor<AWeapon>(VikingShield);
		Bow = World->SpawnActor<ABow>(VikingBow);

		//Cast
		AActor* VikingActor = GetOwner();
		if(!VikingActor){
			UE_LOG(LogTemp, Display, TEXT("Can't Find Owner"));
			return;
		}

		ACharacter* VikingCharacter = Cast<ACharacter>(VikingActor);
		APawn* VikingPawn = Cast<APawn>(VikingActor);

		if(!VikingCharacter || !VikingPawn){
			UE_LOG(LogTemp, Display, TEXT("Can't find Viking (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}
	
		if(Axe){
			Axe->Equip(VikingCharacter->GetMesh(), FName("SpineSocket_Axe"), VikingActor, VikingPawn);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Viking Axes (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}

		if(Shield){
			Shield->Equip(VikingCharacter->GetMesh(), FName("SpineSocket_Shield"), VikingActor, VikingPawn);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Viking Shield (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}

		if(Bow){
			Bow->Equip(VikingCharacter->GetMesh(), FName("LeftHandBowSocket"), VikingActor, VikingPawn);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Viking Bow (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}
	}
	
}

void UVikingWeapon::SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	if(CollisionWeapon && CollisionWeapon->GetWeaponBox())
	{	
		CollisionWeapon->OverlappedActorClear();
		CollisionWeapon->IgnoreActors.Add(GetOwner());

		CollisionWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
	}else if(!CollisionWeapon){
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Weapon (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}


// Called every frame
void UVikingWeapon::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

