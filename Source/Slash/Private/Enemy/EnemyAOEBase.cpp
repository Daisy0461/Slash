// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAOEBase.h"
#include "Components/CapsuleComponent.h"
#include "Interfaces/HitInterface.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEnemyAOEBase::AEnemyAOEBase()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	RootComponent = SceneComponent;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleComp->SetupAttachment(RootComponent);
	AOEEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AOE Effect"));
	AOEEffect->SetupAttachment(RootComponent);

	AOEEffect->bAutoActivate = bIsAutoActive;
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnemyAOEBase::BeginPlay()
{
	Super::BeginPlay();
	
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAOEBase::OnAOECapsuleOverlap); 

	if (CapsuleComp && bDrawAOECapsule)
	{
		DrawDebugCapsule(
			GetWorld(),
			CapsuleComp->GetComponentLocation(),                 
			CapsuleComp->GetScaledCapsuleHalfHeight(),           
			CapsuleComp->GetScaledCapsuleRadius(),               
			FQuat::Identity.Rotator().Quaternion(),             
			FColor::Red,                                        
			false,                                               
			DrawTime,                                            
			0,                                                   
			1.0f 
		);
	}
}

// Called every frame
void AEnemyAOEBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyAOEBase::OnAOECapsuleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{	
	if(!OtherActor) return;
	UE_LOG(LogTemp, Display, TEXT("AOE Begin Overlap : %s"), *OtherActor->GetName());
	if(bIsIgnoreSelf && OtherActor == this) return;		//자기 자신을 무시하면 OtherActor가 자신일 때 무시
	if(bIsIgnoreEnemy && OtherActor->ActorHasTag(TEXT("Enemy"))) return;	//같은 Enemy를 무시한다면 OtherActor가 Enemy일 때 무시
}

void AEnemyAOEBase::DestroyAOE()
{
	Destroy();
}



