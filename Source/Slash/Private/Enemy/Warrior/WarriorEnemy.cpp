// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorEnemy.h"
#include "Enemy/Warrior/WarriorWeapon.h"

AWarriorEnemy::AWarriorEnemy()
{
    WarriorWeapon = CreateDefaultSubobject<UWarriorWeapon>(TEXT("WarriorWeapon"));
}

void AWarriorEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWarriorEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AWarriorEnemy::AttackByAI()
{
    Super::AttackByAI();   		//Play AutoAttack Montage

}




