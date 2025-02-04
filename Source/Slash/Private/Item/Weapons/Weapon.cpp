#include "Item/Weapons/Weapon.h"
#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Enemy/EnemyInterface.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
    WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
    WeaponBox -> SetupAttachment(ItemMesh);
    WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    ParryBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Parry Box"));
    ParryBox -> SetupAttachment(ItemMesh);
    ParryBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ParryBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
    ParryBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

    BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(ItemMesh);
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(ItemMesh);

    PrimaryActorTick.bCanEverTick = false;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxOverlap); 
    ParryBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnParryBoxOverlap);
}   

void AWeapon::OverlappedActorClear()
{
    WeaponBoxOverlappedActors.Empty();
}

void AWeapon::OnWeaponBoxOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if(!GetOwner()){
        UE_LOG(LogTemp, Warning, TEXT("Get Owner is null (%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }
    //UE_LOG(LogTemp, Display, TEXT("Weapon Overlap : %s"), *OtherActor->GetName());

    if(!AttackActor){
        AttackActor = GetOwner();
    }

    // if(WeaponBoxOverlappedActors.Contains(OtherActor)){
    //     UE_LOG(LogTemp, Warning, TEXT("already overrlaped Actor : %s (%s)"), *OtherActor->GetName(), *FPaths::GetCleanFilename(__FILE__));
    // }

    if (!OtherActor || WeaponBoxOverlappedActors.Contains(OtherActor) || 
        ActorIsSameEnemyType(OtherActor) || GetOwner() == OtherActor) return;
        

    TArray<FHitResult> HitResults; // Hit 결과를 저장할 배열
    HitTrace(HitResults); // Multi로 HitTrace 호출

    WeaponBoxOverlappedActors.Add(OtherActor);

    // Hit 결과를 순회하며 처리

    for (const FHitResult& BoxHit : HitResults)
    {
        AActor* BoxHitActor = BoxHit.GetActor();
        //UE_LOG(LogTemp, Display, TEXT("BoxHitActor : %s"), *BoxHitActor->GetName());
        // IHitInterface* HitResultHitInterface;
        // if(BoxHitActor) HitResultHitInterface = Cast<IHitInterface>(BoxHitActor);
        //UE_LOG(LogTemp, Display, TEXT("Box Trace Hit Actor : %s"), *BoxHit.GetActor()->GetName());
        
        //현재는 그냥 Overlap된 Actor와 Hit Trace Actor가 동일한지를 확인하네.
        if (BoxHitActor == OtherActor)
        {
            ABaseCharacter* OtherCharacter = Cast<ABaseCharacter>(OtherActor);
            if(!OtherCharacter) continue;
            
            if (ActorIsSameEnemyType(BoxHit.GetActor())) {
                continue;
            }
            UGameplayStatics::ApplyDamage(
                OtherActor,
                Damage,
                GetInstigator()->GetController(),
                this,
                UDamageType::StaticClass()
            );

            // IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor);
            // if(EnemyInterface){     //적이라면 
            //     FName HitBoneName = BoxHit.BoneName;
            //     UE_LOG(LogTemp, Display, TEXT("Hit Bone Name : %s"), *HitBoneName.ToString());

            //     EnemyInterface->BreakSkeletalBone(BoxHit.ImpactPoint, BoxHit.ImpactNormal, HitBoneName);
            // }
            
            //AOE도 부딪히면 HitStop이 발동함. -> 여러 마리의 Enemy가 있을 때를 대비해서 무시해야함.
            HittedActor = BoxHitActor;
            StartHitStop(Damage);
            SpawnWeaponParticle(BoxHit.ImpactPoint);
            HitInterface(BoxHit);       //여기서 GetHit으로 들어간다.
            CreateFields(BoxHit.ImpactPoint);

            return;
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
        //첫번째 매개변수는 해당 함수를 실행시킬 객체지정
        HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
    }
}

bool AWeapon::ActorIsSameEnemyType(AActor* OtherActor)
{
    return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::OnParryBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    //if(!bIsParryCollisionEnable) return;

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
    //ItemState = EItemState::EIS_Equipped;

    SetOwner(NewOwner);
    SetInstigator(NewInstigator);
    AttachMeshToSocket(InParent, InSocketName);
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
    FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}
void AWeapon::SpawnWeaponParticle(const FVector &ImpactPoint)
{
	if(HitParticles && GetWorld()){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles, 
			ImpactPoint
		);
	}
}

void AWeapon::StartHitStop(const float DamageAmount)
{
    if(!useHitStop) return;
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
    //UE_LOG(LogTemp, Display, TEXT("End Hit Stop"));
    HittedActor->CustomTimeDilation = 1.0f;
    ParryInterface = Cast<IParryInterface>(AttackActor);
    if(ParryInterface && ParryInterface->GetIsParryDilation()){
        AttackActor->CustomTimeDilation = 5.f;
    }else{
        AttackActor->CustomTimeDilation = 1.f;
    }
}
