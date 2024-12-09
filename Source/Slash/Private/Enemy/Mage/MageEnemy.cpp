// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageEnemy.h"
#include "Enemy/Mage/MageEnemyAIController.h"
#include "Enemy/EnemyAreaHeal.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"       //속도 변환용 헤더파일
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AMageEnemy::AMageEnemy()
{
    AttackRadius = 2000.f;
    DefendRadius = 3000.f;

    FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FireBall Position"));
    FirePosition->SetupAttachment(GetRootComponent());
    TeleportNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Teleport Niagara"));
    TeleportNiagaraComp->SetupAttachment(GetRootComponent());
    TeleportNiagaraComp->bAutoActivate = false;

    GetCharacterMovement()->MaxFlySpeed = TeleportSpeed;
    //GetCharacterMovement()->MaxAcceleration = 10000.f;      //처음부터 MaxFlySpeed로 가게끔 함. -> 이런 값이 굳이 필요할까?
    PrimaryActorTick.bCanEverTick = true;
}

void AMageEnemy::BeginPlay()
{
    Super::BeginPlay();

    USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
    if (!SkeletalMeshComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Skeletal Mesh Component not found!"));
        return;
    }

    int32 MaterialCount = SkeletalMeshComponent->GetNumMaterials();
    for (int32 i = 0; i < MaterialCount; ++i)
    {
        UMaterialInterface* Material = SkeletalMeshComponent->GetMaterial(i);
        if (Material)
        {
            //UMaterialInstanceDynamic을 생성해야 런타임에서 동적으로 변환 가능.
            UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
            if (DynamicMaterial)
            {
                // Set the Dynamic Material Instance back to the mesh
                SkeletalMeshComponent->SetMaterial(i, DynamicMaterial);
                DynamicMaterials.Add(DynamicMaterial);
            }else{
                UE_LOG(LogTemp, Warning, TEXT("Material Create Fail : %d"), i);
            }
            
        }
    }
}

void AMageEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(isFading){
        //UE_LOG(LogTemp, Display, TEXT("In Mage Fade Tick"));
        FadeElapsedTime += DeltaTime;

        float Alpha = FMath::Clamp(FadeElapsedTime / FadeDuration, 0.0f, 1.0f);      
        float NewOpacity = FMath::Lerp(CurrentOpacity, TargetOpacity, Alpha);

        for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
        {
            if (DynamicMaterial)
            {
                DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), NewOpacity);
            }
        }


        if (Alpha >= 1.0f)
        {
            isFading = false;
            SpawnTeleportEffets(false);
            IgnoreCollision(false);
        }
    }
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
    TeleportFadeIn();
    
}

void AMageEnemy::HideMesh(bool doHide)
{
    if(GetMesh()){
        if (doHide)
        {
            for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
            {
                if (DynamicMaterial)
                {
                    DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
                }
            }

            HideHealthBar(); 
        }
        else
        {
            TeleportFadeIn();
        }
    }
}

void AMageEnemy::SpawnTeleportEffets(bool doSpawn)
{
    if(doSpawn && TeleportEffect && GetMesh()){
        if(!TeleportBodyEffectComp){
            TeleportBodyEffectComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffect, GetMesh(), FName("spine_01"));
        }
        if(!TeleportEffectTrailComp){
            TeleportEffectTrailComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffectTrail, GetMesh(), FName("spine_01"));
        }
    }else if(!doSpawn){
        if(TeleportBodyEffectComp){
            //UE_LOG(LogTemp, Display, TEXT("Destory Body Effect"));
            TeleportBodyEffectComp->DestroyComponent();
            TeleportBodyEffectComp = nullptr;
        }
        if(TeleportEffectTrailComp){
            //UE_LOG(LogTemp, Display, TEXT("Destory Trail Effect"));
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

void AMageEnemy::TeleportFadeIn()
{
    if (DynamicMaterials.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No dynamic materials to fade!"));
        return;
    }

    //UE_LOG(LogTemp, Display, TEXT("Call Teleport Fade In"));

    isFading = true;
    FadeElapsedTime = 0.0f;
    CurrentOpacity = 0.0f;  
    TargetOpacity = 1.0f;   
}

void AMageEnemy::ActivateTeleportNiagara()
{
    //UE_LOG(LogTemp, Display, TEXT("Call TeleportNiagra Activate"));
    if(TeleportNiagaraComp){
        //UE_LOG(LogTemp, Display, TEXT("In Activate In"));
        TeleportNiagaraComp->Activate();
    }
}

void AMageEnemy::DeactivateTeleportNiagara()
{
    //UE_LOG(LogTemp, Display, TEXT("Call TeleportNiagra Deactivate"));
    if(TeleportNiagaraComp){
        //SpawnTeleportEffets(false);
        //UE_LOG(LogTemp, Display, TEXT("In Deactivate If"));
        TeleportNiagaraComp->Deactivate();
        //TeleportNiagaraComp->ResetSystem();
    }
}

void AMageEnemy::MageHealing()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(MageHealingMontage && HealingAreaClass && AnimInstance){
        //UE_LOG(LogTemp, Display, TEXT("In MageHealing"));
        AnimInstance->Montage_Play(MageHealingMontage);
        HealingArea = GetWorld()->SpawnActor<AEnemyAreaHeal>(HealingAreaClass, GetGroundLocation(), GetActorRotation());

        //CreateUObject가 Delegate 생성이다.
        FOnMontageBlendingOutStarted BlendingOutDelegate = FOnMontageBlendingOutStarted::CreateUObject(this, &AMageEnemy::OnMontageBlendingOut);
        AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MageHealingMontage);
    }
}

FVector AMageEnemy::GetGroundLocation()
{
    FVector StartLocation = GetActorLocation();
    FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f);
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
    {
        return HitResult.Location; // 바닥 위치 반환
    }

    return StartLocation - FVector(0.f, 0.f, 20.f);
}

void AMageEnemy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage == MageHealingMontage)
    {
        // HealingArea 삭제
        DestroyHealingArea();


        // if (bInterrupted)   UE_LOG(LogTemp, Warning, TEXT("MageHealingMontage interrupted!"));
        // else UE_LOG(LogTemp, Log, TEXT("MageHealingMontage finished successfully."));

    }
}

void AMageEnemy::DestroyHealingArea()
{
    if(HealingArea){
        HealingArea->Destroy();
        HealingArea = nullptr;
    }
}