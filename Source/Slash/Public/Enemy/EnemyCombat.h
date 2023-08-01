
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyCombat.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyCombat();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Attack Time")
	float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Attack Time")
	float AttackMax = 0.5f;

protected:
	virtual void BeginPlay() override;

private:
	APawn* ParentActor;

};
