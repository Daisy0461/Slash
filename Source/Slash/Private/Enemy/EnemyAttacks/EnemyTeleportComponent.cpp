
#include "Enemy/EnemyAttacks/EnemyTeleportComponent.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"
#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

//이걸 사용하기 위해서는 Matrial의 Opacity수정이 필요하다.
UEnemyTeleportComponent::UEnemyTeleportComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerEnemy = Cast<AEnemy>(GetOwner());
}

void UEnemyTeleportComponent::EnemyTeleportFunction(EEnemyTeleportEnum TeleportFunctionType)
{
    switch (TeleportFunctionType)
    {
    case EEnemyTeleportEnum::EETE_StartTeleport:
        //UE_LOG(LogTemp, Log, TEXT("StartTeleport executed"));
        StartTeleport(); // Start teleport logic
        break;

    case EEnemyTeleportEnum::EETE_EndTeleport:
        //UE_LOG(LogTemp, Log, TEXT("EndTeleport executed"));
        EndTeleport(); // End teleport logic
        break;

    case EEnemyTeleportEnum::EETE_ActivateTeleportNiagara:
        //UE_LOG(LogTemp, Log, TEXT("ActivateTeleportNiagara executed"));
        ActivateTeleportNiagara(); // Activate teleport Niagara effect
        break;

    case EEnemyTeleportEnum::EETE_DeactivateTeleportNiagara:
        //UE_LOG(LogTemp, Log, TEXT("DeactivateTeleportNiagara executed"));
        DeactivateTeleportNiagara(); // Deactivate teleport Niagara effect
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid Teleport Function Type"));
        break;
    }
}

void UEnemyTeleportComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!OwnerEnemy){
		AActor* Owner = GetOwner();
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr : %s (%s)"), *Owner->GetName(), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	if (NiagaraSystemAsset && GetOwner())
    {
        // Niagara Component를 동적으로 생성
        TeleportNiagaraComp = NewObject<UNiagaraComponent>(GetOwner(), UNiagaraComponent::StaticClass(), TEXT("Teleport Niagara"));
        if (TeleportNiagaraComp)
        {
            TeleportNiagaraComp->SetupAttachment(GetOwner()->GetRootComponent());
            TeleportNiagaraComp->SetAsset(NiagaraSystemAsset);
            TeleportNiagaraComp->bAutoActivate = false; 
            TeleportNiagaraComp->RegisterComponent(); 	//이거 관련해서 알아보자.
        }
    }

	//Mesh를 동적으로 변환하기 위한 단계
	USkeletalMeshComponent* SkeletalMeshComponent = OwnerEnemy->GetMesh();
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

void UEnemyTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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

void UEnemyTeleportComponent::StartTeleport()
{
	HideMesh(true);
    IgnoreCollision(true);
    SpawnTeleportEffets(true);
}

void UEnemyTeleportComponent::EndTeleport()
{
	HideMesh(false);
    TeleportFadeIn();
}

void UEnemyTeleportComponent::ActivateTeleportNiagara()
{
	if(TeleportNiagaraComp)
	{
		TeleportNiagaraComp->Activate();
	}
}

void UEnemyTeleportComponent::DeactivateTeleportNiagara()
{
	if(TeleportNiagaraComp)
	{
		TeleportNiagaraComp->Deactivate();
	}
}

void UEnemyTeleportComponent::HideMesh(bool doHide)
{
	    if(OwnerEnemy->GetMesh()){
        if (doHide)
        {
            for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
            {
                if (DynamicMaterial)
                {
                    DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), 0.0f);
                }
            }

            OwnerEnemy->HideHealthBar(); //Enemy에서 public으로 전환 필요
            //UE_LOG(LogTemp, Display, TEXT("In Hide Mesh true"));
        }
        else
        {
			//ShowMesh인데.. 결국 여기가..
            TeleportFadeIn();
        }
    }
}

void UEnemyTeleportComponent::SpawnTeleportEffets(bool doSpawn)
{
	if(doSpawn && TeleportEffect && OwnerEnemy->GetMesh()){
        if(!TeleportBodyEffectComp){
            TeleportBodyEffectComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffect, OwnerEnemy->GetMesh(), FName("spine_01"));
        }
        if(!TeleportEffectTrailComp){
            TeleportEffectTrailComp = UGameplayStatics::SpawnEmitterAttached(TeleportEffectTrail, OwnerEnemy->GetMesh(), FName("spine_01"));
        }
    }else if(!doSpawn){
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

void UEnemyTeleportComponent::IgnoreCollision(bool doIgnore)
{
    if(doIgnore){
        OwnerEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
        OwnerEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        OwnerEnemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    }else{      //원상복구
        OwnerEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
        OwnerEnemy->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        OwnerEnemy->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
    }
}

void UEnemyTeleportComponent::TeleportFadeIn()
{
    if (DynamicMaterials.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No dynamic materials to fade!"));
        return;
    }

    //UE_LOG(LogTemp, Display, TEXT("Call Teleport Fade In"));

    isFading = true;
    FadeElapsedTime = 0.0f;		//이걸 FadeDuration과 동일하게 하면 바로 나타남.
    CurrentOpacity = 0.0f;  	//이것도 Target과 동일하게 하면 됌.
    TargetOpacity = 1.0f;   

	if(bIsFadeImmediately){
		FadeElapsedTime = FadeDuration;
		CurrentOpacity = TargetOpacity;
	}
}