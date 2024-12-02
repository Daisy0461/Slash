// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageEnemy.h"
#include "Enemy/Mage/MageEnemyAIController.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"       //속도 변환용 헤더파일
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

AMageEnemy::AMageEnemy()
{
    AttackRadius = 800.f;
    DefendRadius = 900.f;

    FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FireBall Position"));
    FirePosition->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->MaxFlySpeed = TeleportSpeed;
    //GetCharacterMovement()->MaxAcceleration = 10000.f;      //처음부터 MaxFlySpeed로 가게끔 함. -> 이런 값이 굳이 필요할까?
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

void AMageEnemy::SpawnFireBall()
{
    UWorld* World = GetWorld();
    if(!FireBall || !FirePosition || !World) return;

    const FVector SpawnLocation = FirePosition->GetComponentLocation();
	AActor* SpawnedFireBall = World->SpawnActor<AActor>(FireBall, SpawnLocation, GetActorRotation());
    if(SpawnedFireBall){
        SpawnedFireBall->SetOwner(this);
        SpawnedFireBall->SetInstigator(Cast<APawn>(this));
    }
}

void AMageEnemy::StartTeleport()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
    HideMesh(true);
    IgnoreCollision(true);
    SpawnTeleportEffets(true);
}

void AMageEnemy::EndTeleport()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    HideMesh(false);
    IgnoreCollision(false);
    SpawnTeleportEffets(false);
}

void AMageEnemy::HideMesh(bool doHide)
{
    if(GetMesh()){
        if(doHide){
            GetMesh()->SetVisibility(false);
            HideHealthBar();
        }else{      //원상복구
            GetMesh()->SetVisibility(true);
        }
    }
}

void AMageEnemy::SpawnTeleportEffets(bool doSpawn)
{
    if(doSpawn && TeleportEffect && GetMesh()){
        TeleportBodyEffectComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffect, GetMesh(), FName("spine_01"));
        TeleportEffectTrailComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffectTrail, GetMesh(), FName("spine_01"));
    }else if(!doSpawn){
        //UE_LOG(LogTemp, Display, TEXT("In Destroy Comp"));
        if(TeleportBodyEffectComp){
            TeleportBodyEffectComp->DestroyComponent();
            TeleportBodyEffectComp = nullptr;
        }
        if(TeleportEffectTrailComp){
            TeleportEffectTrailComp->DestroyComponent();
            TeleportEffectTrailComp = nullptr;
        }
        
    }
}

void AMageEnemy::IgnoreCollision(bool doIgnore)
{
    if(doIgnore){
        GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
        GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    }else{      //원상복구
        GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
        GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
    }
}