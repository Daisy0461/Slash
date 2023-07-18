// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyMoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyMoveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool InTargetRange(AActor* Target, double Radius);

	FORCEINLINE double GetCombatRadius() const {return CombatRadius;};
private:
	void MoveToTarget(AActor* Target);
	void CheckPatrolTarget();
	
	AActor* ChoosePatrolTarget();
	APawn* ParentActor;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY()
	class AAIController* EnemyController;
	//Navigation
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
};
