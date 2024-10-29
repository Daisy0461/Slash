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

void AWarriorEnemy::EnemyGuard()
{
    UE_LOG(LogTemp, Display, TEXT("EnemyGuarding"));
    SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Idle);
    FName GuardingSection = TEXT("EnemyGuarding");
    ChoosePlayMontageSection(GuardingAnimation, GuardingSection);
    isEnemyGuarding = true;
}

void AWarriorEnemy::EnemyGuardImpact()
{
    isEnemyGuarding = false;
    FName GuardImpactSection = TEXT("EnemyGuardImpact");
    ChoosePlayMontageSection(GuardImpactAnimation, GuardImpactSection);
}

void AWarriorEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    UE_LOG(LogTemp, Display, TEXT("In Get Hit Warrior Version"));
    if(isEnemyGuarding){
        //여기서 실행하니까 실질적으로 맞지 않았을 때 움직이지 않는 오류가 발생한다.
        EnemyGuardImpact();
        SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Sprinting);
        return;
    }else{
	    Super::GetHit_Implementation(ImpactPoint, Hitter);
    }
}


