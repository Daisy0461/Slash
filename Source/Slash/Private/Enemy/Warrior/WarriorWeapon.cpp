// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorWeapon.h"
#include "GameFramework/Character.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "GameFramework/Pawn.h"
#include "Item/Weapons/Weapon.h"

// Sets default values for this component's properties
UWarriorWeapon::UWarriorWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWarriorWeapon::BeginPlay()
{
	Super::BeginPlay();

	//Weapon & Shield 장착
	UWorld* World = GetWorld();

	if(World && (WarriorWeapon || WarriorShield)){
		AWeapon* Weapon = World->SpawnActor<AWeapon>(WarriorWeapon);
		AWeapon* Shield = World->SpawnActor<AWeapon>(WarriorShield);

		//Cast
		AActor* WarriorActor = GetOwner();
		if(!WarriorActor){
			UE_LOG(LogTemp, Display, TEXT("Can't Find Owner"));
			return;
		}

		ACharacter* WarriorCharactor = Cast<ACharacter>(WarriorActor);
		APawn* WarriorPawn = Cast<APawn>(WarriorActor);
		AWarriorEnemy* WarriorEnemy = Cast<AWarriorEnemy>(WarriorActor);

		if(!WarriorCharactor || !WarriorPawn || !WarriorEnemy){
			UE_LOG(LogTemp, Display, TEXT("Can't find Warrior"));
			return;
		}
	
		if(Weapon){
			Weapon->Equip(WarriorCharactor->GetMesh(), FName("WeaponSocket"), WarriorActor, WarriorPawn);
			WarriorEnemy->SetEquippedWeapon(Weapon);
		}else{
			UE_LOG(LogTemp, Display, TEXT("Can't Find Warrior Weapon"));
		}

		if(Shield){
			Shield->Equip(WarriorCharactor->GetMesh(), FName("LeftHandSocket"), WarriorActor, WarriorPawn);
		}else{
			UE_LOG(LogTemp, Display, TEXT("Can't Find Warrior Shield"));
		}
	}
}