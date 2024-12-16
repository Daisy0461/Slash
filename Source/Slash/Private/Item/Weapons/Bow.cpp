#include "Item/Weapons/Bow.h"
#include "Item/Weapons/Arrow.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Enemy/Warrior/EnemyGuardInterface.h"
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
        //UE_LOG(LogTemp, Display, TEXT("Bow TimelineUpdate"));
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

    if(!InitOwner()){
        UE_LOG(LogTemp, Warning, TEXT("Bow Init Owner is Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    CameraComponent = NewOwner->FindComponentByClass<UCameraComponent>();
    if(CameraComponent){
        SpringArm = Cast<USpringArmComponent>(CameraComponent->GetAttachParent());
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
    ArrowDamagePercent = 0.f;

    if(GetWorld()){
        GetWorld()->GetTimerManager().SetTimer(AimTimerHandle, this, &ABow::IncreaseDrawTime, DrawIncreaseTime, true);
    }
}

void ABow::VikingBowShot()
{
    if(!isSpawnArrow) return;
    if(!OwnerController){
        UE_LOG(LogTemp, Warning, TEXT("In Bow Shot Owner Controller is Null (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    APlayerController* OwnerPlayerController = Cast<APlayerController>(OwnerController);

    int32 ViewportSizeX, ViewportSizeY;
    OwnerPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
    FVector2D ScreenLocation(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);

    FHitResult HitResult;
    //화면 좌표에서 RayCast를 함.
    bool bHit = OwnerPlayerController->GetHitResultAtScreenPosition(
        ScreenLocation,  
        ECC_WorldDynamic,  //Overlap 안됌. Block이어야함.
        false,           
        HitResult        
    );

    FVector DirectionVector;

    if (bHit) {		//RayCast가 맞았다면
        DirectionVector = HitResult.ImpactPoint;
        AActor* HitActor = HitResult.GetActor();
        if(HitActor){
            //UE_LOG(LogTemp, Warning, TEXT("BowShot RayCast Hit Actor : %s" ), *HitActor->GetName());
            IEnemyGuardInterface* EnemyGaurdInterface = Cast<IEnemyGuardInterface>(HitActor);
            if(EnemyGaurdInterface){
                //UE_LOG(LogTemp, Display, TEXT("In Hit Interface"));
                EnemyGaurdInterface->EnemyGuard(this);
            }
        }
        //DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Red, false, 1.0f);
    }
    else {
        //UE_LOG(LogTemp, Warning, TEXT("Bow Shot RayCast Not Hit"));
        FVector CameraLocation;
        FRotator CameraRotation;
        OwnerPlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
        DirectionVector = CameraLocation + CameraRotation.Vector() * 10000.0f;
        //DrawDebugSphere(GetWorld(), DirectionVector, 10.0f, 12, FColor::Blue, false, 1.0f);
    }

    FVector ArrowLocation = GetArrowLocation();
    FVector UnitDirectionVector = (DirectionVector - ArrowLocation).GetSafeNormal();

    FireArrow(UnitDirectionVector);
}

void ABow::IncreaseDrawTime()
{
    DrawTime += DrawIncreaseTime;
    //Indicator에 Broadcast
    //UE_LOG(LogTemp, Display, TEXT("IncreaseDrawTime : %f (%s)"), DrawTime, *FPaths::GetCleanFilename(__FILE__));
    OnAimOngoing.Broadcast(MaxDrawTime, DrawTime);

    ArrowDamagePercent = FMath::Clamp(DrawTime / MaxDrawTime, 0.0f, 1.0f);
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

	Arrow->SetArrowFire(Direction, ClampAimValue, ArrowDamagePercent);
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

bool ABow::InitOwner()
{
    OwnerActor = GetOwner();
    if(!OwnerActor) {
        UE_LOG(LogTemp, Warning, TEXT("Bow Get Owner is Null (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return false;
    }

    OwnerCharacter = Cast<ACharacter>(OwnerActor);
    if (!OwnerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner is not a Character. (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return false;
    }

    OwnerController = OwnerCharacter->GetController();
    if (!OwnerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Owner has no controller. (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return false;
    }

    return true;
}