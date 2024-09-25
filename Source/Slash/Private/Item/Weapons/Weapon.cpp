// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox -> SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //아래 코드는 모든 체크박스를 Overlap으로 체크하는 것이다.
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);  
}

void AWeapon::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::CapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::CapsuleEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::CapsuleEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (ActorIsSameType(OtherActor) || GetOwner() == OtherActor) {
        return;
    }
    
    TArray<FHitResult> HitResults; // Hit 결과를 저장할 배열
    HitTrace(HitResults); // Multi로 HitTrace 호출

    // 빨간색으로 Debug되는 존재가 OtherActor이다.
    //UE_LOG(LogTemp, Display, TEXT("OnBoxOverlap Name: %s"), *OtherActor->GetName());

    // Hit 결과를 순회하며 처리
    for (const FHitResult& BoxHit : HitResults)
    {
        AActor* BoxHitActor = BoxHit.GetActor();
        IHitInterface* HitResultHitInterface;
        if(BoxHitActor) HitResultHitInterface = Cast<IHitInterface>(BoxHitActor);

        if (BoxHitActor == OtherActor)
        {
            UE_LOG(LogTemp, Display, TEXT("BoxHitActor == OtherActor"));
            if (ActorIsSameType(BoxHit.GetActor())) {
                continue;
            }

            UGameplayStatics::ApplyDamage(
                OtherActor, 
                Damage,
                GetInstigator()->GetController(),
                this,
                UDamageType::StaticClass()
            );
            
            HitInterface(BoxHit);
            CreateFields(BoxHit.ImpactPoint);
        }else{
            UE_LOG(LogTemp, Display, TEXT("BoxHitActor != OtherActor"));
        }
    }
}

void AWeapon::HitTrace(TArray<FHitResult>& HitResults)
{
    int32 testInt = 0;
    const FVector Start = BoxTraceStart->GetComponentLocation(); // 월드 위치 얻기
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore; // 무시할 액터 목록
    ActorsToIgnore.Add(this);
    ActorsToIgnore.AddUnique(GetOwner());

    // 무시할 액터 추가
    for (AActor* Actor : IgnoreActors) {
        ActorsToIgnore.AddUnique(Actor);
    }

    // 박스 트레이스를 실행하여 여러 결과를 얻는다.
    TArray<FHitResult> AllHitResults; // 모든 히트 결과를 저장할 배열
    UKismetSystemLibrary::BoxTraceMulti(
        this, // 컨텍스트
        Start, // 시작 위치
        End, // 끝 위치
        WeaponBoxTraceExtend, // 박스 크기
        BoxTraceStart->GetComponentRotation(), // 박스 회전
        UEngineTypes::ConvertToTraceType(ECC_Pawn), // 트레이스 타입
        true, // 지면에만 충돌
        ActorsToIgnore, // 무시할 액터 목록
        bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, // 디버그 표시
        AllHitResults, // 결과를 저장할 배열
        true // 지면만 무시
    );

    // 중복된 액터를 제거하기 위해 TSet을 사용
    TSet<AActor*> UniqueActors;
    for (const FHitResult& Hit : AllHitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && !UniqueActors.Contains(HitActor)) {
            HitResults.Add(Hit); // 중복되지 않은 HitResult만 추가
            UniqueActors.Add(HitActor); // 액터를 TSet에 추가
        }
    }

    for (const FHitResult& Hit : HitResults)
    {
        UE_LOG(LogTemp, Display, TEXT("BoxTraceResult : %s // TestInt = %d"), *Hit.GetActor()->GetName(), testInt);
    }
}

void AWeapon::HitInterface(const FHitResult& BoxHit)
{
    IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
    if (HitInterface) {
        HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
    }
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
    return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    ItemState = EItemState::EIS_Equipped;

    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}