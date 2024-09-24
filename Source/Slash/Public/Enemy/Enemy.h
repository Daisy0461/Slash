// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy/EnemyInterface.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "Enemy.generated.h"

class UHealthBarComponent; 
class UEnemyMoveComponent;
class UPawnSensingComponent; 
class UEnemyCombat;
class UBehaviorTree;
class UBlackboardData;
class AHealth;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	//아래는 HitInterface의 GetHit을 override한 것이다. 그렇다면 GetHit은 virtual이 아닌데 왜 가능하냐면
	//GetHit이 BlueprintNativeEvent이기 때문이다. 이것은 BP에서도 사용할 수 있고 C++에서도 override해서 구현할 수 있도록 하는 기능이다.
	//GetHit이라는 함수 이름에 _Implementation만 붙여주면 된다.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	EEnemyState GetEnemyState();
	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree();

	//Attack
	virtual void AttackByAI() override;
	virtual void SetAIAttackDelegate(const FAIEnemyAttackFinished& InOnAttackFinished) override;

	//Movement
    void SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed);
    EEnemyMovementSpeed GetMovementSpeedEnum() const;

	
	//Patroll
	UPROPERTY(EditAnywhere)
	AActor* PatrollSpline;
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetPatrolRoute() const override;


protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual void SetHitting() override;
	virtual void GetHittingEnd() override;
	FName SelectDieAnimation();

	//HitStop
	void StartHitStop(float DamageAmount, AActor* PlayerActor);
	void EndHitStop();
	FTimerHandle HitStopTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Stop")
	float HitStopModifier = 1.f;		//damage에 따라 다른 시간을 적용하기 위해 사용

	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	

private:	
	//HealthBar
	void HideHealthBar();
	void ShowHealthBar();
	
	bool IsChasing();
	bool IsParryed();
	bool IsGetHitting();
	bool IsAttacking();
	bool IsEngage();
	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Combat");
	float DestoryTime = 8.f;

	//Behavior Tree
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditAnywhere)
	UBlackboardData* BlackBoard;

	//Attack Time
	FTimerHandle AttackTimer;

	//Attack Radius
	UPROPERTY(EditAnywhere)
	double MotionWarpAttackRadius = 650.f;
	UPROPERTY(EditAnywhere)
	double AutoAttackRadius = 350.f;

	//Attack Move
	virtual float CheckTargetDistance(); 
	virtual void AttackRotate();


	//Parry
	UFUNCTION(BlueprintCallable)
	void ParryCheck();
	UFUNCTION(BlueprintCallable)
	void ParryStunEnd();
	bool isParryed = false;
	FTimerHandle ParryTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* ParryedMontage;
	FTimerHandle ParryedTimerHandle;
	

	//Components
	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	UPROPERTY(VisibleAnywhere)
	UEnemyCombat* EnemyCombat;

	//Heal Item Spawn
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AHealth> HealthClass;
	void SpawnHealItem();

	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EEnemyMovementSpeed CurrentMovementSpeed = EEnemyMovementSpeed::EEMS_Idle;

};
