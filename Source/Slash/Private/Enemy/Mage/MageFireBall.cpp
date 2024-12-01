// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Mage/MageFireBall.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AMageFireBall::AMageFireBall()
{
 	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMageFireBall::OnBoxBeginOverlap);
	RootComponent = BoxComponent;
	FireBallEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Fire Ball Effect"));
	FireBallEffect->SetupAttachment(GetRootComponent());

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
    BoxTraceStart->SetupAttachment(GetRootComponent());
    BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
    BoxTraceEnd->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMageFireBall::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	GetWorldTimerManager().SetTimer(DestroyFireBallTimeHandle, this,&AMageFireBall::DestroyFireBall,FireBallDuration,false);
}

void AMageFireBall::Tick(float DeltaTime)
{
	//Fire Ball Move
	//UE_LOG(LogTemp, Display, TEXT("Fire Ball Tick"));
	FVector ForwardDirection = GetActorForwardVector();
	//UE_LOG(LogTemp, Log, TEXT("Forward Vector: %s"), *ForwardDirection.ToString());
    FVector NewLocation = GetActorLocation() + (ForwardDirection * FireBallSpeed * DeltaTime);
    SetActorLocation(NewLocation);
}

void AMageFireBall::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor->ActorHasTag(TEXT("Enemy"))) return;

	TArray<FHitResult> HitResults; // Hit 결과를 저장할 배열
    HitTrace(HitResults); // Multi로 HitTrace 호출

	for (const FHitResult& BoxHit : HitResults)
    {
        AActor* BoxHitActor = BoxHit.GetActor();
		IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);

        if (BoxHitActor == OtherActor && HitInterface)
        {
            if(!GetInstigator() || !GetInstigator()->GetController()){
				UE_LOG(LogTemp, Display, TEXT("Get Instigator is null"));
				return;
			}

            UGameplayStatics::ApplyDamage(
                OtherActor,
                Damage,
                GetInstigator()->GetController(),
                this,
                UDamageType::StaticClass()
            );
            
            HitInterface->Execute_GetHit(OtherActor, BoxHit.ImpactPoint, GetOwner());

            return;
        }
    }

	PrimaryActorTick.bCanEverTick = false;
	SpawnHitParticle();
	DestroyFireBall();
	return;
}

void AMageFireBall::HitTrace(TArray<FHitResult>& HitResults)
{
    // int32 testInt = 0;
    // const FVector Start = BoxTraceStart->GetComponentLocation(); // 월드 위치 얻기
    // const FVector End = BoxTraceEnd->GetComponentLocation();

    // TArray<AActor*> ActorsToIgnore; // 무시할 액터 목록
    // ActorsToIgnore.Add(this);
    // ActorsToIgnore.AddUnique(GetOwner());

    // // 박스 트레이스를 실행하여 여러 결과를 얻는다.
    // TArray<FHitResult> AllHitResults; // 모든 히트 결과를 저장할 배열
    // UKismetSystemLibrary::BoxTraceMulti(
    //     this, Start, End, 
    //     WeaponBoxTraceExtend, 
    //     BoxTraceStart->GetComponentRotation(), 
    //     UEngineTypes::ConvertToTraceType(ECC_Pawn), 
    //     true, 
    //     ActorsToIgnore, 
    //     bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
    //     AllHitResults, 
    //     true 
    // );

    // // 중복된 액터를 제거하기 위해 TSet을 사용
    // TSet<AActor*> UniqueActors;
    // for (const FHitResult& Hit : AllHitResults)
    // {
    //     AActor* HitActor = Hit.GetActor();
    //     if (HitActor && !UniqueActors.Contains(HitActor)) {
    //         HitResults.Add(Hit); // 중복되지 않은 HitResult만 추가
    //         UniqueActors.Add(HitActor);
    //     }
    // }
}

void AMageFireBall::SpawnHitParticle()
{
	if(!FireBallHitEffect) return;

	FVector HitLocation = GetActorLocation(); // 예시: 현재 Actor의 위치
    FRotator HitRotation = GetActorRotation(); // 예시: 현재 Actor의 회전

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireBallHitEffect, HitLocation, HitRotation);
}

void AMageFireBall::DestroyFireBall()
{
	Destroy();
}




