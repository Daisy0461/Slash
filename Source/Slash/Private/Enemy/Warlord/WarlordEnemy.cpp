// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordEnemy.h"
#include "Enemy/Warlord/WarlordWeapon.h"
#include "Enemy/Warlord/WarlordEnemyAIController.h"
#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"


AWarlordEnemy::AWarlordEnemy()
{
    WarlordWeapon = CreateDefaultSubobject<UWarlordWeapon>(TEXT("WarlordWeapon"));
    WarlordWeapon->bAutoActivate = true;
}

void AWarlordEnemy::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Display, TEXT("WarlordEnemy BeginPlay"));

    WarlordEnemyAIController = Cast<AWarlordEnemyAIController>(this->GetController());
}

void AWarlordEnemy::SetWarlordWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarlordWeapon->SetWeaponCollision(CollisionWeapon, CollisionType);
}

void AWarlordEnemy::SetWarlordParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
    WarlordWeapon->SetParryBoxCollision(CollisionWeapon, CollisionType);
}

AWeapon* AWarlordEnemy::GetWarlordWeapon()
{
    return WarlordWeapon->GetWeapon();
}