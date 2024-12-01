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
    AttackRadius = 900.f;
    DefendRadius = 1050.f;

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

void AMageEnemy::Teleport(FVector NewLocation)
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
    HideMesh(true);
    IgnoreCollision(true);
    
    if(TeleportEffect && GetMesh()){
        UParticleSystemComponent* TeleportBodyEffect = UGameplayStatics::SpawnEmitterAttached(TeleportEffect, GetMesh(), FName("spine_01"));
        UParticleSystemComponent* TeleportTrailEffect = UGameplayStatics::SpawnEmitterAttached(TeleportEffectTrail, GetMesh(), FName("spine_01"));
    }

    AAIController* AIController = Cast<AAIController>(this->GetController());
    if(AIController){
        AIController->MoveToLocation(NewLocation);
    }else{
        UE_LOG(LogTemp, Display, TEXT("Can't find AIC"));
    }
}

void AMageEnemy::EndTeleport()
{
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    HideMesh(false);
    IgnoreCollision(false);
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

