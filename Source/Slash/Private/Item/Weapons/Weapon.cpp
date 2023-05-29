// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapons/Weapon.h"
#include "Character/VikingCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox -> SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    //아래 코드는 모든 체크박스를 Overlap으로 체크하는 것이다.
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    //채널 하나하나 어떤것에 체크할지 정하는 것이다. 아래는 Pawn이라는 Object Type에 대해서 Ignore하겠다는 의미이다.
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::BoxOverlap);  
}

void AWeapon::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::CapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    AVikingCharacter* VikingCharacter_Overlap = Cast<AVikingCharacter>(OtherActor);
    if(VikingCharacter_Overlap){
        VikingCharacter = VikingCharacter_Overlap;
    }
}

void AWeapon::CapsuleEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::CapsuleEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::BoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();        //이렇게 하면 World Location을 얻게된다.
    //BoxTraceStart -> GetRelativeLocation(); //이건 Local Location을 얻어온다.
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;     //TArray는 <>안에 있는 Type을 담을 수 있으며 크기는 넣는 것 만큼 동적으로 커진다.
    ActorsToIgnore.Add(this);
    ActorsToIgnore.Add(VikingCharacter);
    ActorsToIgnore.Add(GetOwner());

    for(AActor* Actor : IgnoreActors){
        ActorsToIgnore.AddUnique(Actor);
    }
    FHitResult BoxHit;
    //BoxTraceSingle은 최초로 부딪힌 것만 처리한다.
    UKismetSystemLibrary::BoxTraceSingle(this, Start, End,
                                        FVector(15.f, 5.f, 30.f),
                                        BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
                                        //UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldDynamic),
                                        false,
                                        ActorsToIgnore,
                                        EDrawDebugTrace::None,
                                        BoxHit, //여기서는 BoxHit에 값을 넣는 역할을 한다.
                                        true
                                        );
    if(BoxHit.GetActor())
    {
        //UE_LOG(LogTemp, Display, TEXT("Box Hit Actor Name: %s"), *BoxHit.GetActor()->GetName());
        IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
        if(HitInterface){
            //HitInterface->GetHit(BoxHit.ImpactPoint);       //BlueprintNativeEvent가 아니라면 이 줄만 있어도 정상적으로 실행이 된다.
            //BlueprintNativeEvent를 사용할 때 기억해야하는 것이 GetHit이라는 BlueprintNativeEvent를 Call했으면 Excute도 해줘야한다는 것이다.

            HitInterface->Execute_GetHit(BoxHit.GetActor(),BoxHit.ImpactPoint);
        }
        IgnoreActors.AddUnique(BoxHit.GetActor());

        CreateFields(BoxHit.ImpactPoint);
    }
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    //이 Item(Weapon)의 Mesh를 Parents에게 붙이고 FName으로 들어온 Socket에 붙이겠다는 의미이다.
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
    ItemState = EItemState::EIS_Equipped;
    // if(Capsule){
    //     //이후에 다시 Overlapp되서 혼동이 발생하는걸 막는 것이다.Collision Check박스를 Ignore로 바꾼다고 생각하면 된다.
    //     Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // }
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}