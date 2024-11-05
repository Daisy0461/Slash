// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Enemy/Warrior/EnemyGuardInterface.h"
#include "WarriorEnemy.generated.h"

class UWarriorWeapon;
class UAnimMontage;
class AWarriorEnemyAIController;

UCLASS()
class SLASH_API AWarriorEnemy : public AEnemy, public IEnemyGuardInterface
{
	GENERATED_BODY()

public:
	AWarriorEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EnemyGuard(AActor* AttackActor) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
protected:
	

private:
	AWarriorEnemyAIController* WarriorEnemyAIController;

	UPROPERTY(VisibleAnywhere)
	UWarriorWeapon* WarriorWeapon;
	UFUNCTION(BlueprintCallable)
	virtual void AttackByAI() override;

	void EnemyGuardImpact();

	bool isEnemyGuarding = false;
	FName GuardingSection = TEXT("EnemyGuarding");
	FName GuardImpactSection = TEXT("EnemyGuardImpact");
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montage")
	UAnimMontage* GuardingAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montage")
	UAnimMontage* GuardImpactAnimation;


};
