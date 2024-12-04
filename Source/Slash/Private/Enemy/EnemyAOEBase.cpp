// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAOEBase.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEnemyAOEBase::AEnemyAOEBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	RootComponent = SphereComp;
}

// Called when the game starts or when spawned
void AEnemyAOEBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAOEBase::OnAOESphereOverlap); 

	if (SphereComp)
	{
		DrawDebugSphere(
			GetWorld(),
			SphereComp->GetComponentLocation(), 
			SphereComp->GetScaledSphereRadius(), 
			12, 
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

void AEnemyAOEBase::OnAOESphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(!OtherActor) return;
	UE_LOG(LogTemp, Display, TEXT("AOE Overlap : %s"), *OtherActor->GetName());

	// if(OtherActor == this || OtherActor->ActorHasTag(TEXT("Enemy"))) return;		//Enemy는 무시한다.

	// IHitInterface& HitInterface = Cast<IHitInterface>(OtherActor);
	// if(HitInterface)
	// {
	// 	UGameplayStatics::ApplyDamage(
	// 		HitInterface,
	// 		Damage,
	// 		nullptr, 
	// 		this,
	// 		UDamageType::StaticClass()
	// 	);

	// 	HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint, GetOwner());
	// }
}

