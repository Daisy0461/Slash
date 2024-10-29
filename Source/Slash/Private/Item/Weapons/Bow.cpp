#include "Item/Weapons/Bow.h"
#include "Item/Weapons/Arrow.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"

ABow::ABow()
{
    PrimaryActorTick.bCanEverTick = true;

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Aim Sound Component"));
    AudioComponent -> SetupAttachment(GetRootComponent());
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

        if(AudioComponent->IsPlaying()){
            AudioComponent->Stop();
        }
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
    AimTimeline.Play();

	SpawnArrow();
    if(AudioComponent->Sound){
        AudioComponent->Play();
    }

    DrawTime = 0;

    if(GetWorld()){
        GetWorld()->GetTimerManager().SetTimer(AimTimerHandle, this, &ABow::IncreaseDrawTime, DrawIncreaseTime, true);
    }
}

void ABow::IncreaseDrawTime()
{
    DrawTime += DrawIncreaseTime;
    OnAimOngoing.Broadcast(MaxDrawTime, DrawTime);
}

void ABow::ClearAimTimer()
{  
    if (GetWorld()->GetTimerManager().IsTimerActive(AimTimerHandle))
    {
        GetWorld()->GetTimerManager().ClearTimer(AimTimerHandle);  // 타이머를 취소
        AimTimerHandle.Invalidate();  // 타이머 핸들을 무효화
    }
}

void ABow::StopAiming()
{
    AimTimeline.Reverse(); // 타임라인 반대로 재생 
    if(AudioComponent->IsPlaying()){
        AudioComponent->Stop();
    }

	DestoryArrow();
}

FVector ABow::GetArrowLocation()
{
    if(Arrow){
        return Arrow->GetArrowLocation();
    }else{
        UE_LOG(LogTemp, Warning, TEXT("In Get Arrow Location Can't find Arrow"));
        return FVector(0, 0, 0);
    }
    
}

void ABow::FireArrow(FVector Direction)
{
    //UE_LOG(LogTemp, Warning, TEXT("Fire Arrow"));
	if(!Arrow && isSpawnArrow){
		UE_LOG(LogTemp, Display, TEXT("Can't Find Arrow"));
		return;
	}

    if(ShotSound){
        UGameplayStatics::PlaySoundAtLocation(this, ShotSound, GetActorLocation());
    }

    float NormalizedAimValue = FMath::GetRangePct(0.f, MaxDrawTime, DrawTime);
    float ClampAimValue = FMath::Clamp(NormalizedAimValue, 0.0f, 1.0f);

	Arrow->SetArrowFire(Direction, ClampAimValue);
    ClearAimTimer();
    isSpawnArrow = false;
}

void ABow::SpawnArrow()
{
    UWorld* World = GetWorld();

	if(World && SpawnedArrow){
		Arrow = World->SpawnActor<AArrow>(SpawnedArrow);
        ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
        if (CharacterOwner)
        {
            Arrow->Equip(CharacterOwner->GetMesh(), TEXT("RightHandArrowSocket"), CharacterOwner, CharacterOwner->GetInstigator());
            isSpawnArrow = true;
        }
        else {
            UE_LOG(LogTemp, Error, TEXT("Owner is not of type ACharacter."));
        }
    }
}

void ABow::DestoryArrow()  
{
    if(Arrow && !(Arrow->GetIsFired())){
        Arrow->Destroy();
        Arrow = nullptr;
        isSpawnArrow = false;
        ClearAimTimer();
    }
}