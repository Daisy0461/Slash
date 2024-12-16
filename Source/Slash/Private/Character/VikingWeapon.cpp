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
		VikingActor = GetOwner();
		if(!VikingActor){
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Owner"));
			return;
		}

		VikingCharacter = Cast<ACharacter>(VikingActor);
		VikingPawn = Cast<APawn>(VikingActor);

		if(!VikingCharacter || !VikingPawn){
			UE_LOG(LogTemp, Warning, TEXT("Can't find Viking (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}

		if(Axe && Shield && Bow){
			AttachAxeAndShieldWeapon();
			bIsEquippingAxe = true;
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Viking Weapons (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}
	}
}

bool UVikingWeapon::ChangeEquip()
{
	if(bIsEquippingAxe){
		UE_LOG(LogTemp, Display, TEXT("AttachBow"));
		return AttachBowWeapon();
	}else{
		UE_LOG(LogTemp, Display, TEXT("AttachAxe"));
		return AttachAxeAndShieldWeapon();
	}
}

bool UVikingWeapon::AttachAxeAndShieldWeapon()
{
	if(Axe && Shield && Bow && VikingActor && VikingPawn && VikingCharacter){
		UE_LOG(LogTemp, Display, TEXT("In Axe Attach"));
		Axe->Equip(VikingCharacter->GetMesh(), FName("RightHandAxeSocket"), VikingActor, VikingPawn);
		Shield->Equip(VikingCharacter->GetMesh(), FName("LeftHandShieldSocket"), VikingActor, VikingPawn);
		Bow->Equip(VikingCharacter->GetMesh(), FName("SpineSocket_Bow"), VikingActor, VikingPawn);
		bIsEquippingAxe = true;
	}

	return bIsEquippingAxe;
}

bool UVikingWeapon::AttachBowWeapon()
{
	if(Axe && Shield && Bow && VikingActor && VikingPawn && VikingCharacter){
		UE_LOG(LogTemp, Display, TEXT("In Bow Attach"));
		Axe->Equip(VikingCharacter->GetMesh(), FName("SpineSocket_Axe"), VikingActor, VikingPawn);
		Shield->Equip(VikingCharacter->GetMesh(), FName("SpineSocket_Shield"), VikingActor, VikingPawn);
		Bow->Equip(VikingCharacter->GetMesh(), FName("LeftHandBowSocket"), VikingActor, VikingPawn);
		bIsEquippingAxe = false;
	}

	return bIsEquippingAxe;
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

