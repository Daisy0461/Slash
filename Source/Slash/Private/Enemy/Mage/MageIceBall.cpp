// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageIceBall.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/ParryInterface.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

AMageIceBall::AMageIceBall()
{
    FireBallEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Ice Ball Effect"));
	FireBallEffect->SetupAttachment(GetRootComponent());
    PrimaryActorTick.bCanEverTick = true;
}

void AMageIceBall::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DestroyFireBallTimeHandle, this,&AMageIceBall::DestroyFireBall,FireBallDuration,false);
    useHitStop = false;
}

void AMageIceBall::Tick(float DeltaTime)
{
	//Fire Ball Move
	FVector ForwardDirection = GetActorForwardVector();
    FVector NewLocation = GetActorLocation() + (ForwardDirection * FireBallSpeed * DeltaTime);
    SetActorLocation(NewLocation);
}

void AMageIceBall::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    //UE_LOG(LogTemp, Display, TEXT("MageIceBall Box Overlap"));
    Super::OnWeaponBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if(OtherActor){
        IParryInterface* ParryInterfaceCast = Cast<IParryInterface>(OtherActor);
        if(!ParryInterfaceCast) return;

        SpawnHitParticle();
        DestroyFireBall();
    }
}

void AMageIceBall::SpawnHitParticle()
{
	if(!FireBallHitEffect) return;

	FVector HitLocation = GetActorLocation(); // 예시: 현재 Actor의 위치
    FRotator HitRotation = GetActorRotation(); // 예시: 현재 Actor의 회전

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireBallHitEffect, HitLocation, HitRotation);
}

void AMageIceBall::DestroyFireBall()
{
	Destroy();
}