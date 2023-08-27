// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/CharacterTypes.h"
#include "EnemyMoveComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UEnemyMoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyMoveComponent();

	void MoveToTarget(AActor* Target);
	bool InTargetRange(AActor* Target, double Radius);

	void StopPatrollingTimer();
	void CheckPatrolTarget();

	FORCEINLINE double GetCombatRadius() const {return CombatRadius;};
	FORCEINLINE AActor* GetPatrolTarget() const {return PatrolTarget;};
	FORCEINLINE double GetPatrolingSpeed() const {return PatrolingSpeed;};
	FORCEINLINE double GetChaseSpeed() const {return ChaseSpeed;};

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	APawn* ParentActor;
	UPROPERTY()
	class AAIController* EnemyController;

	//Patrolling
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
	AActor* ChoosePatrolTarget();
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "Combat");
	float PatrolingSpeed = 130.f;

	//Patrolling Time
	void PatrolTimerFinished();
	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 3.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 8.f;


	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Combat");
	double ChaseSpeed = 400.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

};
