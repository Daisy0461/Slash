// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "BaseEnemyAIController.generated.h"

class AEnemy;
class UBehaviorTree;
class UBlackboardComponent;
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
	virtual void SetEnemyState(const EEnemyState State);
	virtual EEnemyState GetEnemyState() const;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	virtual void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
	virtual void GetPerceptionInfo(AActor* Actor);

	AEnemy* Enemy;
	UAIPerceptionComponent* AIPerceptionComponent;
	UBlackboardComponent* BlackboardComponent;
	EEnemyState EnemyState = EEnemyState::EES_Passive;

	FName StateKeyName = TEXT("State");

private:
	//void DrawSightDebug();
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Hearing* HearingConfig;
	UPROPERTY(VisibleAnywhere)
	UAISenseConfig_Damage* DamageConfig;
};
