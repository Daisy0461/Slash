// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warlord/WarlordEnemy.h"
#include "Enemy/Warlord/WarlordWeapon.h"
#include "Enemy/Warlord/WarlordEnemyAIController.h"
#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"


AWarlordEnemy::AWarlordEnemy()
{
    WarlordWeapon = CreateDefaultSubobject<UWarlordWeapon>(TEXT("WarlordWeapon"));
    WarlordWeapon->bAutoActivate = true;

    EnemyAutoAttackComponent = CreateDefaultSubobject<UEnemyAutoAttackComponent>(TEXT("AutoAttackComponent"));
    EnemyAOEAttackComponent = CreateDefaultSubobject<UEnemyAOEAttackComponent>(TEXT("AOEAttackComponent"));
    EnemyFireBallAttackComponent = CreateDefaultSubobject<UEnemyFireBallAttackComponent>(TEXT("FireBallAttackComponent"));
}

void AWarlordEnemy::BeginPlay()
{
    Super::BeginPlay();

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