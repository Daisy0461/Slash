// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Enemy/Warrior/EnemyGuardInterface.h"
#include "WarlordEnemy.generated.h"

class UWarriorWeapon;
class UWarlordWeapon;
class UEnemyAutoAttackComponent;
class UEnemyAOEAttackComponent;
class UEnemyFireBallAttackComponent;
class UEnemyThrowWeaponAttackComponent;
class UEnemyTeleportComponent;
class AWeapon;
class AWarlordEnemyAIController;

UCLASS()
class SLASH_API AWarlordEnemy : public AEnemy, public IEnemyGuardInterface
{
	GENERATED_BODY()
public:
	AWarlordEnemy();
	virtual void EnemyGuard(AActor* AttackActor) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	//Weapon
	UFUNCTION(BlueprintCallable)
	void SetWarlordWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	void SetWarlordParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWarlordWeapon();

	//Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyAutoAttackComponent* EnemyAutoAttackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyAOEAttackComponent* EnemyAOEAttackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyFireBallAttackComponent* EnemyFireBallAttackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyThrowWeaponAttackComponent* EnemyThrowWeaponAttackComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UEnemyTeleportComponent* EnemyTeleportComponent;

private:
	UPROPERTY(VisibleAnywhere)
	UWarlordWeapon* WarlordWeapon;
	AWarlordEnemyAIController* WarlordEnemyAIController;

	//Guard
	bool isEnemyGuarding = false;
	FName GuardingSection = TEXT("EnemyGuarding");
	FName GuardImpactSection = TEXT("EnemyGuardImpact");
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	UAnimMontage* GuardingAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Guard")
	UAnimMontage* GuardImpactAnimation;
	void PlayGuardImpactMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Guard") 
	int32 RandomGaurdPercent = 40;
	bool bIsDoGaurd = false;
	void RandomGaurd();
};
