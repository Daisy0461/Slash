// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyExplosionAttack.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AEnemyExplosionAttack::AEnemyExplosionAttack()
{
    bDrawAOECapsule = false;
    bIsIgnoreEnemy = true;
    CautionAOEEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Caution AOE Effect"));
	CautionAOEEffect->SetupAttachment(RootComponent);

    if(!bIsImmediateDamage){        //true라면 Overlap 시 바로 Damage를 준다.
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        AOEEffect->bAutoActivate = false;
    }
}

void AEnemyExplosionAttack::BeginPlay()
{
    Super::BeginPlay();
    if(AOEEffect)
    {
        //Effect 끝날 때 호출
        AOEEffect->OnSystemFinished.AddDynamic(this, &AEnemyExplosionAttack::OnAOEEffectFinished);
    }

    if(!bIsImmediateDamage){        //경고하고 때린다.
        GetWorldTimerManager().SetTimer(ActiveDamageTimer, this, &AEnemyExplosionAttack::ActiveAOEDamage, CautionEffectDurtaion, false);

    }
}

void AEnemyExplosionAttack::OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::OnAOECapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (OtherActor)
    {
        if (IgnoreActors.Contains(OtherActor))
        {
            UE_LOG(LogTemp, Warning, TEXT("Actor %s is already overlaped"), *OtherActor->GetName());
            return;
        }
        IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
        if(HitInterface){
            HitInterface->GetHitAOEAttack();
            UGameplayStatics::ApplyDamage(
                        OtherActor,
                        AOEDamage,
                        nullptr,
                        this,
                        UDamageType::StaticClass()
                    );

            IgnoreActors.Add(OtherActor);       //2번 데미지 안입기 위해서 추가.
        }
    }
}

void AEnemyExplosionAttack::OnAOEEffectFinished(UParticleSystemComponent* FinishedComponent)
{   
    UE_LOG(LogTemp, Display, TEXT("Effect End"));
    Destroy();
}

void AEnemyExplosionAttack::ActiveAOEDamage()
{
    if(CapsuleComp && CautionAOEEffect && AOEEffect){
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
        CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
        CautionAOEEffect->DestroyComponent();
        AOEEffect->Activate(true);
    }
}


