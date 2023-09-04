// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UEnemyMoveComponent;
class UPawnSensingComponent;
class UEnemyCombat;
class AWeapon;
class AShield;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	//아래는 HitInterface의 GetHit을 override한 것이다. 그렇다면 GetHit은 virtual이 아닌데 왜 가능하냐면
	//GetHit이 BlueprintNativeEvent이기 때문이다. 이것은 BP에서도 사용할 수 있고 C++에서도 override해서 구현할 수 있도록 하는 기능이다.
	//GetHit이라는 함수 이름에 _Implementation만 붙여주면 된다. 그럼 이 함수는 C++에서만 사용하는 함수로 정해준다.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	void Destoryed();

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;
	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual int32 PlayDeathMontage() override;
	virtual void AttackEnd() override;
	FName SelectDieAnimation();

	//HitStop
	void StartHitStop(float DamageAmount, AActor* PlayerActor);
	void EndHitStop();
	FTimerHandle HitStopTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Stop")
	float HitStopModifier = 1.f;		//damage에 따라 다른 시간을 적용하기 위해 사용

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CombatTarget;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AShield> ShieldClass;
	
	//HealthBar
	void HideHealthBar();
	void ShowHealthBar();
	
	//AI 행동
	void CheckCombatTarget();
	void LoseInterest();
	void StartParoling();
	void ChaseTarget();

	//상태 체크
	bool IsOutSideCombatRadius();
	bool IsOutSideAttackRadius();
	bool IsInSideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngage();
	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Combat");
	float DestoryTime = 8.f;


	//Attack
	virtual void Attack() override;

	//Attack Time
	void StartAttackTimer();
	void ClearAttackTimer();
	FTimerHandle AttackTimer;

	//Attack Radius
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;


	//Components
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;
	UPROPERTY(VisibleAnywhere)
	UEnemyMoveComponent* EnemyMove;
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	UPROPERTY(VisibleAnywhere)
	UEnemyCombat* EnemyCombat;

};
