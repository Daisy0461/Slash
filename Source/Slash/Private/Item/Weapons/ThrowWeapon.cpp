// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/ThrowWeapon.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/ParryInterface.h"
#include "Particles/ParticleSystem.h"

AThrowWeapon::AThrowWeapon()
{
    WeaponRotatePivot = CreateDefaultSubobject<USceneComponent>(TEXT("WeawponRotatePivot"));
    WeaponRotatePivot->SetupAttachment(GetRootComponent());
    ThrowWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ThrowWeaponMesh"));
    ThrowWeaponMesh->SetupAttachment(WeaponRotatePivot);

    PrimaryActorTick.bCanEverTick = true;
}

void AThrowWeapon::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(DestroyThrowWeaponTimeHandle, this,&AThrowWeapon::DestroyThrowWeapon,ThrowWeaponDuration,false);
    useHitStop = false;
}

void AThrowWeapon::Tick(float DeltaTime)
{
	FVector ForwardDirection = GetActorForwardVector();
    FVector NewLocation = GetActorLocation() + (ForwardDirection * ThrowWeaponSpeed * DeltaTime);
    SetActorLocation(NewLocation);

    if (WeaponRotatePivot)
    {
        FRotator CurrentRotation = WeaponRotatePivot->GetRelativeRotation();
        float NewZRotation = CurrentRotation.Yaw + (ThrowWeaponRotateSpeed * DeltaTime);
        FRotator NewRotation(0.f, NewZRotation, 0.f); // Pitch = 0 (X축 고정), Roll = 0 (Y축 고정)

        WeaponRotatePivot->SetRelativeRotation(NewRotation);
    }
}

void AThrowWeapon::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    //UE_LOG(LogTemp, Display, TEXT("ThrowWeapon Box Overlap"));
    Super::OnWeaponBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    if(OtherActor){
        IParryInterface* ParryInterfaceCast = Cast<IParryInterface>(OtherActor);
        if(!ParryInterfaceCast) return;

        SpawnHitParticle();
        DestroyThrowWeapon();
    }
}

void AThrowWeapon::SpawnHitParticle()
{
    if(!ThrowWeaponHitEffect) return;

	FVector HitLocation = GetActorLocation(); // 예시: 현재 Actor의 위치
    FRotator HitRotation = GetActorRotation(); // 예시: 현재 Actor의 회전

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrowWeaponHitEffect, HitLocation, HitRotation);
}

void AThrowWeapon::DestroyThrowWeapon()
{
	Destroy();
}