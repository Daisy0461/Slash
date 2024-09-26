#include "Item/Weapons/Weapon.h"
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

void AWeapon::OverlappedActorClear()
{
    OverlappedActors.Empty();
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (!OtherActor || OverlappedActors.Contains(OtherActor) || 
        ActorIsSameType(OtherActor) || GetOwner() == OtherActor) {
        return;
    }

    TArray<FHitResult> HitResults; // Hit 결과를 저장할 배열
    HitTrace(HitResults); // Multi로 HitTrace 호출

    OverlappedActors.Add(OtherActor);

    // Hit 결과를 순회하며 처리
    for (const FHitResult& BoxHit : HitResults)
    {
        AActor* BoxHitActor = BoxHit.GetActor();
        IHitInterface* HitResultHitInterface;
        if(BoxHitActor) HitResultHitInterface = Cast<IHitInterface>(BoxHitActor);

        if (BoxHitActor == OtherActor)
        {
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
            TestHitCount++;
            UE_LOG(LogTemp, Display, TEXT("Hit Count : %d   //  This is %s"), TestHitCount, *OtherComp->GetName());
            
            HitInterface(BoxHit);
            CreateFields(BoxHit.ImpactPoint);
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
        this, Start, End, 
        WeaponBoxTraceExtend, 
        BoxTraceStart->GetComponentRotation(), 
        UEngineTypes::ConvertToTraceType(ECC_Pawn), 
        true, 
        ActorsToIgnore, 
        bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        AllHitResults, 
        true 
    );

    // 중복된 액터를 제거하기 위해 TSet을 사용
    TSet<AActor*> UniqueActors;
    for (const FHitResult& Hit : AllHitResults)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && !UniqueActors.Contains(HitActor)) {
            HitResults.Add(Hit); // 중복되지 않은 HitResult만 추가
            UniqueActors.Add(HitActor);
        }
    }

    // for (const auto Hit : UniqueActors)
    // {
    //     UE_LOG(LogTemp, Display, TEXT("Hit Actor Name : %s"), *Hit->GetName());
    // }
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
void AWeapon::SpawnWeaponParticle()
{
	if(HitParticles && GetWorld()){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles, 
			GetActorLocation()
		);
	}
}