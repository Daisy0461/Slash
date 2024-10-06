#include "Item/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Interfaces/HitInterface.h"
#include "Enemy/EnemyInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox -> SetupAttachment(GetRootComponent());
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    ParryBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Parry Box"));
    ParryBox -> SetupAttachment(GetRootComponent());
    ParryBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ParryBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    ParryBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxOverlap); 
    ParryBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnParryBoxOverlap);
    AttackActor = GetOwner();
}   

void AWeapon::OverlappedActorClear()
{
    WeaponBoxOverlappedActors.Empty();
}

void AWeapon::OnWeaponBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (!OtherActor || WeaponBoxOverlappedActors.Contains(OtherActor) || 
        ActorIsSameType(OtherActor) || GetOwner() == OtherActor) {
        return;
    }
    //UE_LOG(LogTemp, Display, TEXT("Your message"));

    TArray<FHitResult> HitResults; // Hit 결과를 저장할 배열
    HitTrace(HitResults); // Multi로 HitTrace 호출

    WeaponBoxOverlappedActors.Add(OtherActor);

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
            // TestHitCount++;  
            // UE_LOG(LogTemp, Display, TEXT("Hit Count : %d   //  Owner is %s"), TestHitCount, *GetOwner()->GetName());
            //Hit Stop GetOwner & BoxHitActor로 하면 될듯
            AttackActor = GetOwner();
            HittedActor = BoxHitActor;
            StartHitStop(Damage);

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

void AWeapon::OnParryBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    //지금 안되는 이유가 Enable안해서 그런거 같음 ㅇㅇ
    //UE_LOG(LogTemp, Warning, TEXT("ParryBoxOverlap"));
    ParryInterface = Cast<IParryInterface>(OtherActor);
    if(ParryInterface){
        if(ParryInterface->ParryCheck() && AttackActor){        //AttackActor가 Stun이 되는 형식으로 만듦.
            IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(AttackActor);
            if(EnemyInterface){
                //UE_LOG(LogTemp, Display, TEXT("InParry"));
                GetWorldSettings()->SetTimeDilation(0.2f);
                OtherActor->CustomTimeDilation = 5.f;
                //UE_LOG(LogTemp, Display, TEXT("SetParryInterface True"));
                ParryInterface->SetIsParryDilation(true);

                FTimerHandle ParryEndTimer;
                EnemyInterface->PlayStunMontage();

                GetWorldTimerManager().SetTimer(ParryEndTimer, this, &AWeapon::ParryStunEnd, 3.f);
            }
        }
    }
}

void AWeapon::ParryStunEnd()
{
	//UE_LOG(LogTemp, Display, TEXT("Parry Stun End"));
	GetWorldSettings()->SetTimeDilation(1.f);
    ParryInterface->RestoreParryTimeDilation();
    //UE_LOG(LogTemp, Display, TEXT("make False"));
    ParryInterface->SetIsParryDilation(false);
    
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

void AWeapon::StartHitStop(const float DamageAmount)
{
    //UE_LOG(LogTemp, Display, TEXT("Start Hit Stop"));
    if(!AttackActor){
        //UE_LOG(LogTemp, Display, TEXT("Cant find Hitstop AttackActor"));
        return;
    }else if(!HittedActor){
        //UE_LOG(LogTemp, Display, TEXT("Cant find Hitstop HittedActor"));
        return;
    }
    AttackActor->CustomTimeDilation = 0.0f;
    HittedActor->CustomTimeDilation = 0.0f;

    float HitStopTime = DamageAmount * HitStopModifier;
    ParryInterface = Cast<IParryInterface>(AttackActor);
    if(ParryInterface && ParryInterface->GetIsParryDilation()){
        HitStopTime /= 5;
    }

    if(GetWorld()){
        GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &AWeapon::EndHitStop, HitStopTime, false);
	}
}

void AWeapon::EndHitStop()
{
    HittedActor->CustomTimeDilation = 1.0f;
    ParryInterface = Cast<IParryInterface>(AttackActor);
    if(ParryInterface && ParryInterface->GetIsParryDilation()){
        AttackActor->CustomTimeDilation = 5.f;
    }else{
        AttackActor->CustomTimeDilation = 1.f;
    }
}