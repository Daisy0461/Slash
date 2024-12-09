// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAreaHeal.h"
#include "Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"

AEnemyAreaHeal::AEnemyAreaHeal()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAreaHeal::BeginPlay()
{
    Super::BeginPlay();

    CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &AEnemyAreaHeal::OnAOECapsuleEndOverlap);

    //객체 초기화 타이밍 때문에 Delay를 준다.
    FTimerHandle DelayTimerHandle;
    GetWorldTimerManager().SetTimer(DelayTimerHandle, this, &AEnemyAreaHeal::InitializeOverlappingEnemies, 0.1f, false);
}

void AEnemyAreaHeal::InitializeOverlappingEnemies()
{
    TArray<AActor*> OverlappingActors;
    CapsuleComp->GetOverlappingActors(OverlappingActors, AEnemy::StaticClass());

    for (AActor* Actor : OverlappingActors)
    {
        AEnemy* Enemy = Cast<AEnemy>(Actor);
        if (Enemy)
        {
            HealingEnemyActors.Add(Enemy);
        }
    }
}

void AEnemyAreaHeal::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    for (AEnemy* Enemy : HealingEnemyActors){
        if (Enemy){
            Enemy->Healing(DeltaTime* HealAmount);
        }
    }

}

void AEnemyAreaHeal::OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    Super::OnAOECapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if(OtherActor){
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if(Enemy){
            //UE_LOG(LogTemp, Display, TEXT("Enmey Healing Overlaped"));
            HealingEnemyActors.Add(Enemy);
        }
    }
}

void AEnemyAreaHeal::OnAOECapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if(OtherActor){
        AEnemy* Enemy = Cast<AEnemy>(OtherActor);
        if(Enemy){
            HealingEnemyActors.Remove(Enemy);
        }
    }
}