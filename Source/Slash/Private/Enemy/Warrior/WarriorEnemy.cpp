// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Warrior/WarriorEnemy.h"
#include "Enemy/Warrior/WarriorWeapon.h"
#include "Enemy/Warrior/WarriorEnemyAIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"

AWarriorEnemy::AWarriorEnemy()
{
    WarriorWeapon = CreateDefaultSubobject<UWarriorWeapon>(TEXT("WarriorWeapon"));
}

void AWarriorEnemy::BeginPlay()
{
	Super::BeginPlay();
	
    WarriorEnemyAIController = Cast<AWarriorEnemyAIController>(this->GetController());
}

void AWarriorEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}


void AWarriorEnemy::AttackByAI()
{
    Super::AttackByAI();   		//Play AutoAttack Montage

}

//viking에서 이 함수를 실행시킴.
void AWarriorEnemy::EnemyGuard(AActor* AttackActor)
{
    UE_LOG(LogTemp, Display, TEXT("EnemyGuarding"));
    //여기서 이 WarriorEnemy가 Pawn의 AI Controller를 AIC_WarriorEnemy를 갖고 있는지 확인해야한다.
    //그렇다는 말은 WarriorEnemyAIController로 변환해야한다. 이것을 기반으로 만든 것이 AIC_BaseEnemyCPP이니까.
    //물론 먼저 변환해놓는게 더 쌀듯 -> BeginPlay() -> OK
    if(!WarriorEnemyAIController){
        UE_LOG(LogTemp, Warning, TEXT("WarriorEnemyAIController Cast Fail"));
        return;
    }

    AActor* Actor = WarriorEnemyAIController->GetAttackTargetActor();
    if(!Actor){     //현재 AttackActor가 뭔지 모름.
        UE_LOG(LogTemp, Warning, TEXT("Warrior Don't Know AttackTargetActor"));
        return;     //Guard 불가능
    }else{
        WarriorEnemyAIController->SetEnemyGuardState(EEnemyGuardState::EEGS_Guarding);
        ChoosePlayMontageSection(GuardingAnimation, GuardingSection);
        isEnemyGuarding = true;
    }
    
}

void AWarriorEnemy::EnemyGuardImpact()
{
    ChoosePlayMontageSection(GuardImpactAnimation, GuardImpactSection);
    if(WarriorEnemyAIController->GetEnemyGuardState() == EEnemyGuardState::EEGS_Guarding){
        //ChoosePlayMontageSection(GuardingAnimation, GuardingSection);
    }
}

void AWarriorEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
    //UE_LOG(LogTemp, Display, TEXT("In Get Hit Warrior Version"));
    if(WarriorEnemyAIController->GetEnemyGuardState() == EEnemyGuardState::EEGS_Guarding){
        EnemyGuardImpact();
    }else{
	    Super::GetHit_Implementation(ImpactPoint, Hitter);
    }
}