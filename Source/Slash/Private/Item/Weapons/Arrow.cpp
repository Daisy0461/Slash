// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/Arrow.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Math/UnrealMathUtility.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Engine/Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    ArrowBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Arrow Box"));
    ArrowBox -> SetupAttachment(GetRootComponent());
    ArrowBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  // 충돌 및 물리 활성화
    ArrowBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    ArrowBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    
    // 물리 시뮬레이션과 중력 활성화
    ArrowBox->SetSimulatePhysics(true);  
    ArrowBox->SetEnableGravity(true);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.5;
    ProjectileMovementComponent->bAutoActivate = false;

    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Trail Location"));
    SceneComponent->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
}

void AArrow::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FRotator InitialRotation = FRotator(0, -180.0f, 0);  // 원하는 회전 값으로 설정
    SetActorRotation(InitialRotation);
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);

}

void AArrow::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);

	//ArrowBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ArrowBox->SetSimulatePhysics(false);

	if (ProjectileMovementComponent)
    {
        ProjectileMovementComponent->StopMovementImmediately();  // 이동 중지
        ProjectileMovementComponent->Deactivate();  // 비활성화
    }
}

FVector AArrow::GetArrowLocation()
{
    return GetActorLocation();
}

void AArrow::SetArrowFire(FVector Direction, float Strength)
{
	if (ProjectileMovementComponent)
    {
		//UE_LOG(LogTemp, Display, TEXT("Direction : %s"), *Direction.ToString());
		ArrowBox->SetSimulatePhysics(true);
        SpawnAttachedNiagaraSystem();
        float ShotStrength = FMath::Lerp(MinSpeed, MaxSpeed, Strength);
        float Gravity = FMath::Lerp(MaxGravity, MinGravity, Strength);
		ProjectileMovementComponent->Activate();  // 발사할 때 활성화
        ProjectileMovementComponent->Velocity = Direction * ShotStrength;
        ProjectileMovementComponent->ProjectileGravityScale = Gravity;

        isFired = true;
    }
}

void AArrow::SpawnAttachedNiagaraSystem()
{
    if (NiagaraSystem)
    {
        // Niagara 시스템을 스폰하고 루트 컴포넌트에 부착
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
            NiagaraSystem,                      // 스폰할 Niagara 시스템
            SceneComponent,                 // 부착할 컴포넌트 (루트 컴포넌트)
            FName(),                            // 소켓 이름 (비워도 됨)
            FVector::ZeroVector,                // 위치 오프셋
            FRotator::ZeroRotator,              // 회전 오프셋
            EAttachLocation::KeepRelativeOffset,// 부착 방식 (상대 위치 유지)
            true                                // 위치 업데이트 여부
        );

        if (NiagaraComp)
        {
            // Niagara 컴포넌트를 활성화
            NiagaraComp->Activate(true);
        }
    }
}