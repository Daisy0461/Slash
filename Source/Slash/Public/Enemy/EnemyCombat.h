
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCombat.generated.h"

class AEnemy;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyCombat();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	void Attack();

	float AutoAttackDistance = 200.f;
	float LongAttackDistance = 300.f;
private:
	AEnemy* OwnerEnemy;
	
};
