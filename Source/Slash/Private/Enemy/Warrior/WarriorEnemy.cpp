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

void AWarriorEnemy::EnemyGuard(AActor* AttackActor)
{
    UE_LOG(LogTemp, Display, TEXT("EnemyGuarding"));
    //SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Idle); -> 이건 BT에서 조절하자. BTTask_SetMovementSpeed

    //여기서 이 WarriorEnemy가 Pawn의 AI Controller를 AIC_WarriorEnemy를 갖고 있는지 확인해야한다.
    //그렇다는 말은 WarriorEnemyAIController로 변환해야한다. 이것을 기반으로 만든 것이 AIC_BaseEnemyCPP이니까.
    //물론 먼저 변환해놓는게 더 쌀듯 -> BeginPlay() -> OK


    if(!WarriorEnemyAIController){
        UE_LOG(LogTemp, Warning, TEXT("WarriorEnemyAIController Cast Fail"));
        return;
    }

    AActor* Actor = WarriorEnemyAIController->GetAttackTargetActor();
    if(!Actor){     //현재 AttackActor가 뭔지 모름.
        UE_LOG(LogTemp, Display, TEXT("Warrior Don't Know AttackTargetActor"));
        return;     //Guard 불가능
    }
    WarriorEnemyAIController->SetEnemyGuardState(EEnemyGuardState::EEGS_Guarding);

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
    //UE_LOG(LogTemp, Display, TEXT("In Get Hit Warrior Version"));
    if(isEnemyGuarding){
        //여기서 실행하니까 실질적으로 맞지 않았을 때 움직이지 않는 오류가 발생한다.
        EnemyGuardImpact();
        SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Sprinting);
        return;
    }else{
	    Super::GetHit_Implementation(ImpactPoint, Hitter);
    }
}


