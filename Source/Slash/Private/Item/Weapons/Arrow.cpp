// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/Arrow.h"
#include "Components/BoxComponent.h"
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

void AArrow::SetArrowFire(FVector Direction)
{
	if (ProjectileMovementComponent)
    {
		//UE_LOG(LogTemp, Display, TEXT("Direction : %s"), *Direction.ToString());
		ArrowBox->SetSimulatePhysics(true);
		ProjectileMovementComponent->Activate();  // 발사할 때 활성화
        ProjectileMovementComponent->Velocity = Direction * ArrowSpeed;

        isFired = true;
    }
}