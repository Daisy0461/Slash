// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/FloatingPlaform.h"

// Sets default values
AFloatingPlaform::AFloatingPlaform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//밟고 올라가기 위해선 Simulate Physic가 활성화되어야한다.
	FloatingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComponent"));
	RootComponent = FloatingMesh;
}

// Called when the game starts or when spawned
void AFloatingPlaform::BeginPlay()
{
	Super::BeginPlay();
	
	if (FloatingCurve)
    {
        // Timeline 업데이트 함수를 Curve와 바인딩
        FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("TimelineUpdate"));

        FloatingTimeline.AddInterpFloat(FloatingCurve, TimelineProgress);
        FloatingTimeline.SetLooping(false); // 반복 여부 설정
    }

	InitialPosition = GetActorLocation();
	InitialZValue = InitialPosition.Z;
	floatingHeight = InitialZValue + floatingHeight;

	FloatingTimeline.Play();
}

// Called every frame
void AFloatingPlaform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FloatingTimeline.TickTimeline(DeltaTime);
	
}

void AFloatingPlaform::TimelineUpdate(float Value)
{
	float NewZValue = FMath::Lerp(InitialZValue, floatingHeight, Value);
	NewPosition = FVector(InitialPosition.X, InitialPosition.Y, NewZValue);
	SetActorLocation(NewPosition);

	// Z 위치가 floatingHeight에 도달했는지 확인
    if (FMath::IsNearlyEqual(NewZValue, floatingHeight, 0.1f)) // 오차 범위 설정
    {
        // Z축 이동을 잠그기 위해 Constraints 활성화
		UE_LOG(LogTemp, Display, TEXT("in Is Nearly"));
        FloatingMesh->BodyInstance.bLockZTranslation = true;
        //FloatingMesh->UpdatePhysicsProperties();
		FloatingMesh->RecreatePhysicsState(); // Physics 상태를 재생성하여 업데이트 적용

        // Timeline 중지 및 Tick 비활성화로 성능 최적화
        FloatingTimeline.Stop();
    }
}

void AFloatingPlaform::ReverseFloating()
{
	FloatingMesh->BodyInstance.bLockZTranslation = false;
	FloatingMesh->RecreatePhysicsState();
    //FloatingMesh->UpdatePhysicsProperties();
	FloatingTimeline.Reverse();
}