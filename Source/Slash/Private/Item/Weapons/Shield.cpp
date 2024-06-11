// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapons/Shield.h"
#include "Character/VikingCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AShield::AShield()
{
    ShieldBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Shield Box"));
    ShieldBox -> SetupAttachment(GetRootComponent());
    // ShieldBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // //아래 코드는 모든 체크박스를 Overlap으로 체크하는 것이다.
    // ShieldBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    // //채널 하나하나 어떤것에 체크할지 정하는 것이다. 아래는 Pawn이라는 Object Type에 대해서 Ignore하겠다는 의미이다.
    // //ShieldBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AShield::BeginPlay()
{
    Super::BeginPlay();

    ShieldBox->OnComponentBeginOverlap.AddDynamic(this, &AShield::OnBoxOverlap);  
}

void AShield::CapsuleOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    Super::CapsuleOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AShield::CapsuleEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
    Super::CapsuleEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AShield::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{   
    //UE_LOG(LogTemp, Warning, TEXT("Shield OnBoxOverlap"));
    //내 생각에 GetOwner에서 에러 뜸
    if(ActorIsSameType(OtherActor) || GetOwner() == OtherActor){       //적끼리 때리지 않고 자기자신에게 맞지않기 위해 추가했다.
        return;
    }
    
    FHitResult BoxHit;
    HitTrace(BoxHit);

    //UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap Name: %s"), *OtherActor->GetName());

    if(BoxHit.GetActor())
    {
        if(ActorIsSameType(BoxHit.GetActor())){
            return;
        }

        //Damage적용
        UGameplayStatics::ApplyDamage(
            BoxHit.GetActor(), 
            Damage,
            GetInstigator()->GetController(),
            this,
            UDamageType::StaticClass());
        //UE_LOG(LogTemp, Warning, TEXT("Damage"));
        HitInterface(BoxHit);
    }
}

void AShield::HitTrace(FHitResult& BoxHit)
{
    const FVector Start = BoxTraceStart->GetComponentLocation();        //이렇게 하면 World Location을 얻게된다.
    //BoxTraceStart -> GetRelativeLocation(); //이건 Local Location을 얻어온다.
    const FVector End = BoxTraceEnd->GetComponentLocation();

    TArray<AActor*> ActorsToIgnore;     //TArray는 <>안에 있는 Type을 담을 수 있으며 크기는 넣는 것 만큼 동적으로 커진다.
    ActorsToIgnore.Add(this);
    ActorsToIgnore.AddUnique(GetOwner());

    for(AActor* Actor : IgnoreActors){
        ActorsToIgnore.AddUnique(Actor);
    }

    UKismetSystemLibrary::BoxTraceSingle(this, Start, End,
                                        BoxTraceExtend,
                                        BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1,
                                        //UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldDynamic),
                                        false,
                                        ActorsToIgnore,
                                        EDrawDebugTrace::None,
                                        BoxHit, //여기서는 BoxHit에 값을 넣는 역할을 한다.
                                        true
                                        );
    //한번의 공격에 여러번 맞게하지 않기 위해서 아래 줄을 추가한다.
    IgnoreActors.AddUnique(BoxHit.GetActor());
}

void AShield::HitInterface(FHitResult& BoxHit){
    IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
    //HitInterface가 Null으로 나온다. 왜?
    if(HitInterface){
        HitInterface->Execute_GetHit(BoxHit.GetActor(),BoxHit.ImpactPoint, GetOwner());
    }
}

bool AShield::ActorIsSameType(AActor* OtherActor)
{
    return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AShield::SpawnShieldParticle()
{
	if(HitParticles && GetWorld()){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles, 
			GetActorLocation()
		);
	}
}

void AShield::PlayShieldSound(const FVector &ImpactPoint)
{
    if(ShieldSound){
		UGameplayStatics::PlaySoundAtLocation(this, ShieldSound, ImpactPoint);
	}
}

void AShield::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
    ItemState = EItemState::EIS_Equipped;

    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
}

void AShield::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}