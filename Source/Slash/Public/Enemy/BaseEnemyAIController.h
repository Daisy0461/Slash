// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BaseEnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UBlackboardComponent;
class UBlackboardData;
class UBehaviorTreeComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;

UCLASS()
class SLASH_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABaseEnemyAIController();
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetAttackTargetActor() const {return AttackTargetActor;}
	virtual void SetEnemyState(const EEnemyState State);
	virtual EEnemyState GetEnemyState() const;
	FString GetEnemyStateAsString(EEnemyState State);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	UFUNCTION()
	virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	virtual void GetPerceptionInfo(AActor* Actor);

	AEnemy* Enemy;
	UPROPERTY(EditAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(EditAnywhere)
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY(EditDefaultsOnly)
	class UBlackboardData*  BlackboardAsset;
	EEnemyState EnemyState = EEnemyState::EES_Passive;

	FName StateKeyName = TEXT("State");
	FName AttackTargetKeyName = TEXT("AttackTarget");
	FName PointOfInterestKeyName = TEXT("PointOfInterest");

private:
	//void DrawSightDebug();
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Damage* DamageConfig;

	UPROPERTY()
	AActor* AttackTargetActor;

	UFUNCTION()
	void SightSensed(AActor* AttackTarget);

	//Set State
	virtual void SetEnemyStateAsPassive();
	virtual void SetEnemyStateAsInvesting(const FVector InvestingLocation);
	virtual void SetEnemyStateAsChasing();
	virtual void SetEnemyStateAsStrafing();
	virtual void SetEnemyStateAsAttacking(AActor* AttackTarget);
	virtual void SetEnemyStateAsParried();
	virtual void SetEnemyStateAsHitting();
	virtual void SetEnemyStateAsDead();
};
