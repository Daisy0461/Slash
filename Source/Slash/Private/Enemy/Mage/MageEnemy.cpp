// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageEnemy.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "GameFramework/CharacterMovementComponent.h"       //속도 변환용 헤더파일
#include "Enemy/Mage/MageEnemyAIController.h"

AMageEnemy::AMageEnemy()
{
    AttackRadius = 700.f;
    DefendRadius = 750.f;
}

void AMageEnemy::BeginPlay()
{
    Super::BeginPlay();

}

void AMageEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    Super::GetHit_Implementation(ImpactPoint, Hitter);
}

void AMageEnemy::SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed)
{
    CurrentMovementSpeed = NewSpeed;

    // Enum에 따른 실제 이동 속도 설정
    switch (NewSpeed)
    {
        case EEnemyMovementSpeed::EEMS_Idle:
            GetCharacterMovement()->MaxWalkSpeed = 0.0f; 
            break;

        case EEnemyMovementSpeed::EEMS_Walk:
            GetCharacterMovement()->MaxWalkSpeed = 200.0f;  
            break;

        case EEnemyMovementSpeed::EEMS_Jogging:
            GetCharacterMovement()->MaxWalkSpeed = 400.0f; 
            break;

        case EEnemyMovementSpeed::EEMS_Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = 400.0f; 
            break;

        default:
            break;
    }
}

void AMageEnemy::AttackByAI()
{
    if(FireBallMontage)
    {
        FName FireBallSection = TEXT("FireBallSection");
        ChoosePlayMontageSection(FireBallMontage, FireBallSection);
    }
}

