
#include "Enemy/EnemyCombat.h"

UEnemyCombat::UEnemyCombat()
{
	PrimaryComponentTick.bCanEverTick = true;
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

