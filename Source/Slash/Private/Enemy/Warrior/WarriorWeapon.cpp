// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorWeapon.h"
#include "GameFramework/Character.h"
#include "Enemy/Warrior/WarriorEnemy.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
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
		Sword = World->SpawnActor<AWeapon>(WarriorWeapon);
		Shield = World->SpawnActor<AWeapon>(WarriorShield);

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
	
		if(Sword){
			Sword->Equip(WarriorCharactor->GetMesh(), FName("WeaponSocket"), WarriorActor, WarriorPawn);
			//WarriorEnemy->SetEquippedWeapon(Weapon);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Warrior Sword"));
		}

		if(Shield){
			Shield->Equip(WarriorCharactor->GetMesh(), FName("LeftHandSocket"), WarriorActor, WarriorPawn);
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Can't Find Warrior Shield"));
		}

		AEnemy* Enemy = Cast<AEnemy>(WarriorEnemy);
		if(!Enemy){
			UE_LOG(LogTemp, Warning, TEXT("In WarriorWeapon Enemy Cast Fail"));
		}else{
			DestoryTime = Enemy->GetDestoryTime();
			Enemy->OnEnemyDeath.AddDynamic(this, &UWarriorWeapon::DestoryWeapons);
		}
	}
}

// void AWarriorEnemy::SwordAttackEnd()
// {
// 	//UE_LOG(LogTemp, Display, TEXT("AttackEnd"));
// 	OnAttackFinished.ExecuteIfBound();
// 	if(Sword){ 
// 		Sword->OverlappedActorClear();
// 	}
// }B



//이후에 bool로 변경
void UWarriorWeapon::SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	UE_LOG(LogTemp, Display, TEXT("Set weapon Collision"), *FPaths::GetCleanFilename(__FILE__));
	if(CollisionWeapon && CollisionWeapon->GetWeaponBox())
	{	
		UE_LOG(LogTemp, Display, TEXT("Before IgnoreActor.Empty (%s)"), *FPaths::GetCleanFilename(__FILE__));
		CollisionWeapon->OverlappedActorClear();
		UE_LOG(LogTemp, Display, TEXT("After IgnoreActor.Empty (%s)"), *FPaths::GetCleanFilename(__FILE__));
		CollisionWeapon->IgnoreActors.Add(GetOwner());

		CollisionWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
	}else if(!CollisionWeapon){
		UE_LOG(LogTemp, Warning, TEXT("Can't Find Weapon (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UWarriorWeapon::SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
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

void UWarriorWeapon::DestoryWeapons()
{
	Sword->SetLifeSpan(DestoryTime);
	Shield->SetLifeSpan(DestoryTime);
}

