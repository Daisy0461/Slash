// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapons/Bow.h"
#include "Item/Weapons/Arrow.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"

ABow::ABow()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABow::BeginPlay()
{
    Super::BeginPlay();

    if (AimCurve)
    {
        // Timeline 업데이트 함수를 Curve와 바인딩
        FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("TimelineUpdate"));

        AimTimeline.AddInterpFloat(AimCurve, TimelineProgress);
        AimTimeline.SetLooping(false); // 반복 여부 설정
    }
    
}

void ABow::Tick(float DeltaTime)
{
    // Timeline을 매 프레임마다 업데이트
    AimTimeline.TickTimeline(DeltaTime);
}

void ABow::TimelineUpdate(float Value)
{
    // 카메라의 FOV를 Lerp로 변경
    float NewFOV = FMath::Lerp(InitialFieldOfView, AimFieldOfView, Value);
    // 카메라의 위치를 Lerp로 변경
    FVector NewCameraOffset = FMath::Lerp(InitialCameraOffset, AimCameraOffset, Value);

    if (CameraComponent)
    {
        // 카메라 FOV와 위치를 직접 업데이트
        CameraComponent->SetFieldOfView(NewFOV);
        SpringArm->SocketOffset = NewCameraOffset;
    }
}

void ABow::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void ABow::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);

    CameraComponent = NewOwner->FindComponentByClass<UCameraComponent>();
    if(CameraComponent){
        SpringArm = Cast<USpringArmComponent>(CameraComponent->GetAttachParent());
        if(SpringArm){
            UE_LOG(LogTemp, Display, TEXT("Find Spring Arm"));
        }
    }
}

void ABow::StartAiming()
{
    AimTimeline.Play(); // 타임라인 재생
    SpawnArrow();
}

void ABow::StopAiming()
{
    AimTimeline.Reverse(); // 타임라인 반대로 재생
    DestoryArrow();
}

void ABow::SpawnArrow()
{
    UWorld* World = GetWorld();

	if(World && SpawnedArrow){
		Arrow = World->SpawnActor<AArrow>(SpawnedArrow);
        ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
        if (CharacterOwner)
        {
            UE_LOG(LogTemp, Display, TEXT("ArrowEquip"));
            Arrow->Equip(CharacterOwner->GetMesh(), TEXT("RightHandArrowSocket"), CharacterOwner, CharacterOwner->GetInstigator());
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Owner is not of type ACharacter."));
        }
    }
}

void ABow::DestoryArrow()
{
    if(Arrow){
        Arrow->Destroy();
        Arrow = nullptr;
    }
}