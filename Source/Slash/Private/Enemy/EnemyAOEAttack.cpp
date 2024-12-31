// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAOEAttack.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/ParryInterface.h"
#include "Kismet/GameplayStatics.h"

AEnemyAOEAttack::AEnemyAOEAttack()
{
    CautionAOEEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Caution AOE Effect"));
	CautionAOEEffect->SetupAttachment(RootComponent);

    AOEEffect->bAutoActivate = true;
    
}

void AEnemyAOEAttack::BeginPlay()
{
    Super::BeginPlay();
    
    //CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAOEBase::OnAOECapsuleOverlap);
    CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AEnemyAOEAttack::OnAOECapsuleEndOverlap);
    if(bIsImmediateDamage){
        AOEEffect->Activate();
        GetWorldTimerManager().SetTimer(AOEDestroyTimer, this, &AEnemyAOEAttack::DestroyAOE, DamageDurationTime, false);
    }else{
        AOEEffect->Deactivate();
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        GetWorldTimerManager().SetTimer(AOEDestroyTimer, this, &AEnemyAOEAttack::ActiveAOEDamage, CautionDuration, false);
    }
}

void AEnemyAOEAttack::OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnAOECapsuleOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (OtherActor)
    { 
        IParryInterface* ParryInterface = Cast<IParryInterface>(OtherActor);
        if(!ParryInterface) return;

        UE_LOG(LogTemp, Display, TEXT("AOE Capsule Overlap Actor : %s (%s)"), *OtherActor->GetName(), *FPaths::GetCleanFilename(__FILE__));
        IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
        if(HitInterface){
            UE_LOG(LogTemp, Display, TEXT("Find HitInterface (%s)"), *FPaths::GetCleanFilename(__FILE__));

            DamageActor(HitInterface, OtherActor);
            GetWorldTimerManager().SetTimer(AOEDamageTimer, [this, HitInterface, OtherActor]()
            {
                this->DamageActor(HitInterface, OtherActor);
            }, 1.0f, true);
        }else{
            UE_LOG(LogTemp, Warning, TEXT("HitInterface is nullptr Actor : %s(%s)"), *OtherActor->GetName(), *FPaths::GetCleanFilename(__FILE__));
        }
    }
}

void AEnemyAOEAttack::OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if(OtherActor){
        IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
        if(HitInterface){
            GetWorldTimerManager().ClearTimer(AOEDamageTimer);
        }
    }
}

void AEnemyAOEAttack::DamageActor(IHitInterface* HitInterface, AActor* DamagedActor)
{
    UE_LOG(LogTemp, Display, TEXT("DamaeActor In"));

    if(!bIsPlayHitReaction){
        HitInterface->GetHitAOEAttack();
    }else{
        //UE_LOG(LogTemp, Display, TEXT("In bIsPlayHitReaction == true"));
        HitInterface->Execute_GetHit(DamagedActor, GetActorLocation(), GetOwner());
    }

    UGameplayStatics::ApplyDamage(
                DamagedActor,
                AOEDamage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
}

void AEnemyAOEAttack::ActiveAOEDamage()
{   
    if(CapsuleComp && CautionAOEEffect && AOEEffect){
        //실제 데미지주는 Effect삭제하는 함수
        GetWorldTimerManager().SetTimer(AOEDestroyTimer, this, &AEnemyAOEAttack::DestroyAOE, DamageDurationTime, false);
        //Overlap되도록 교체 
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        CautionAOEEffect->DestroyComponent();
        AOEEffect->Activate(true);
    }
}

void AEnemyAOEAttack::DestroyAOE()
{
    Destroy();
}