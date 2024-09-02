// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorEnemy.h"
#include "Enemy/Warrior/WarriorWeapon.h"

AWarriorEnemy::AWarriorEnemy()
{
    WarriorWeapon = CreateDefaultSubobject<UWarriorWeapon>(TEXT("WarriorWeapon"));
}

void AWarriorEnemy::Attack()
{
    //Super::Attack();

    UE_LOG(LogTemp, Display, TEXT("Attack In CPP"));
	if(AutoAttackMontage){
		UE_LOG(LogTemp, Display, TEXT("In AutoAttack"));
		PlayAutoAttackMontage();
	}
}