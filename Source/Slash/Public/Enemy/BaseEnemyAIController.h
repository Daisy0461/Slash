// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BaseEnemyAIController.generated.h"

class AEnemy;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBlackboardData;
class UBrainComponent;
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
	//virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetAttackTargetActor() const {return AttackTargetActor;}
	virtual void SetEnemyState(const EEnemyState State);

	//Set State
	UFUNCTION(BlueprintCallable)
	virtual EEnemyState GetEnemyState() const;
	FString GetEnemyStateAsString(EEnemyState State);
	virtual void SetEnemyStateAsPassive();
	virtual void SetEnemyStateAsInvesting(const FVector InvestingLocation);
	virtual void SetEnemyStateAsChasing();
	virtual void SetEnemyStateAsStrafing();
	virtual void SetEnemyStateAsAttacking(AActor* AttackTarget);
	virtual void SetEnemyStateAsParried();
	virtual void SetEnemyStateAsHitting();
	virtual void SetEnemyStateAsDead();

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
	UBlackboardData*  BlackboardAsset;
	UPROPERTY(EditDefaultsOnly)
	UBehaviorTreeComponent* BTComp;
	// UPROPERTY(EditDefaultsOnly)
	// UBrainComponent* BrainComp;
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
	UFUNCTION()
    void OnEnemyDied(); // 적이 죽었을 때 호출할 함수

	UPROPERTY()
	AActor* AttackTargetActor;

	UFUNCTION()
	void SightSensed(AActor* AttackTarget);
};
