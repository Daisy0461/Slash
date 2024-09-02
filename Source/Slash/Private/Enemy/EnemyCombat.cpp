
#include "Enemy/EnemyCombat.h"
#include "Enemy/Enemy.h"

UEnemyCombat::UEnemyCombat()
{
	PrimaryComponentTick.bCanEverTick = true;
	OwnerEnemy = Cast<AEnemy>(GetOwner());
}

void UEnemyCombat::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void UEnemyCombat::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnemyCombat::Attack()
{
	//AnimationMontage 재생
	UE_LOG(LogTemp, Display, TEXT("Attack In CPP"));
	// if(OwnerEnemy && AutoAttackMontage){
	// 	//UE_LOG(LogTemp, Display, TEXT("In AutoAttack"));
	// 	PlayAutoAttackMontage();
	// }
}

