// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageAOEAttack.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

void AMageAOEAttack::BeginPlay()
{
    Super::BeginPlay();
    bDrawAOECapsule = true;
    // bIsIgnoreSelf = true;
    // bIsIgnoreSameEnemy = true;

    //CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAOEBase::OnAOECapsuleOverlap);
    CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AMageAOEAttack::OnAOECapsuleEndOverlap);
    GetWorldTimerManager().SetTimer(AOEDestroyTimer, this, &AMageAOEAttack::DestroyAOE, DestroyTime, false);
}

void AMageAOEAttack::OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnAOECapsuleOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (OtherActor)
    {
        IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
        if(HitInterface){
            GetWorldTimerManager().SetTimer(AOEDamageTimer, [this, HitInterface, OtherActor]()
            {
                this->DamageActor(HitInterface, OtherActor);
            }, 1.0f, true);
        }
    }
}

void AMageAOEAttack::OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if(OtherActor){
        IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
        if(HitInterface){
            GetWorldTimerManager().ClearTimer(AOEDamageTimer);
        }
    }
}

void AMageAOEAttack::DamageActor(IHitInterface* HitInterface, AActor* DamagedActor)
{
    HitInterface->GetHitAOEAttack();
    UGameplayStatics::ApplyDamage(
                DamagedActor,
                AOEDamage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
}
