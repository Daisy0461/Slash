// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordWeapon.h"
#include "Enemy/Enemy.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Item/Weapons/Weapon.h"

// Sets default values for this component's properties
UWarlordWeapon::UWarlordWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UWarlordWeapon::BeginPlay()
{
	UE_LOG(LogTemp, Display, TEXT("WarlordWeaponBeginPlay _BS"));
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("WarlordWeaponBeginPlay"));
	//Weapon & Shield 장착
	UWorld* World = GetWorld();

	if(World && WarlordWeapon){
		Sword = World->SpawnActor<AWeapon>(WarlordWeapon);

		//Cast
		AActor* WarlordActor = GetOwner();
		if(!WarlordActor){
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Owner (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}

		WarlordCharactor = Cast<ACharacter>(WarlordActor);
		WarlordPawn = Cast<APawn>(WarlordActor);

		if(!WarlordCharactor || !WarlordPawn){
			UE_LOG(LogTemp, Warning, TEXT("Can't find Warlord (%s)"), *FPaths::GetCleanFilename(__FILE__));
			return;
		}
	
		if(Sword){
			UE_LOG(LogTemp, Display, TEXT("Equip Before"));
			Sword->Equip(WarlordCharactor->GetMesh(), FName("WarlordWeaponSocket"), WarlordActor, WarlordPawn);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Warlord Sword (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}

		AEnemy* Enemy = Cast<AEnemy>(WarlordActor);
		if(!Enemy){
			UE_LOG(LogTemp, Warning, TEXT("In WarlUWarlordWeapon Enemy Cast Fail"));
		}else{
			DestoryTime = Enemy->GetDestoryTime();
			Enemy->OnEnemyDeath.AddDynamic(this, &UWarlordWeapon::DestoryWeapons);
		}
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Warlord Weapon is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UWarlordWeapon::SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
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

void UWarlordWeapon::SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	if(CollisionWeapon && CollisionWeapon->GetParryBox())
	{	
		CollisionWeapon->OverlappedActorClear();
		CollisionWeapon->IgnoreActors.Add(GetOwner());

		CollisionWeapon->GetParryBox()->SetCollisionEnabled(CollisionType);
	}else if(!CollisionWeapon){
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Weapon in ParryBoxCollision (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UWarlordWeapon::DestoryWeapons()
{
	Sword->SetLifeSpan(DestoryTime);
}
