// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Components/TimelineComponent.h"
#include "Enemy/EnemyInterface.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "Enemy.generated.h"

class UHealthBarComponent; 
class UEnemyMoveComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UEnemyCombat;
class UCurveFloat;
class AVikingGameState;
class UBehaviorTree;
class UBlackboardComponent;
class UBlackboardData;
class UMetaSoundSource;
class AHealth;
class AWeapon;
class ABaseEnemyAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHit);

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
	virtual void GetHeadShot(FVector ImpactPoint) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Passive;
	
	EEnemyState GetEnemyState();
	FORCEINLINE void SetEnemyState(EEnemyState State) {EnemyState = State;}
	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree();
	FORCEINLINE uint8 GetTeamID() {return TeamID; }
	//UAIPerceptionComponent* GetAIPerceptionComponent() const;
	UBlackboardComponent* GetBlackboardComponent() const;
	float GetAttackRadius() const;
	FORCEINLINE void SetAttackRadius(float Radius) {AttackRadius = Radius;};
	float GetDefendRadius() const;
	FORCEINLINE void SetDefendRadius(float Radius) {DefendRadius = Radius;};

	//Attack
	AVikingGameState* VikingGameState;
	virtual void AttackByAI() override;
	virtual void SetAIAttackDelegate(const FAIEnemyAttackFinished& InOnAttackFinished) override;
	UPROPERTY()
	FOnEnemyDeath OnEnemyDeath;
	UPROPERTY()
	FOnEnemyHit OnEnemyHit;


	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EEnemyMovementSpeed CurrentMovementSpeed = EEnemyMovementSpeed::EEMS_Idle;
    virtual void SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed);
    EEnemyMovementSpeed GetMovementSpeedEnum() const;
	virtual void StopMovement();
	
	//Patroll NOTE::COMP
	UPROPERTY(EditAnywhere)
	AActor* PatrollSpline;
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetPatrolRoute() const override;

	//Weapon
	virtual AWeapon* GetWeapon() override;
	FORCEINLINE float GetDestoryTime() {return DestoryTime; }

	//Heal
	virtual void Healing(float HealAmount) override;
protected:
	UPROPERTY(EditAnywhere, Category = "Combat");
	float AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = "Combat");
	float DefendRadius = 500.f;

	virtual void BeginPlay() override;
	virtual void Die() override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual void SetHitting() override;
	virtual void GetHittingEnd() override;

	//EnmeyDie
	virtual void BreakSkeletalBone(FVector ImpactPoint, FVector ImpactNormal, FName BreakBoneName) override;
	FName SelectDieAnimation();

	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	
	//HealthBar
	void HideHealthBar();
	void ShowHealthBar();

	//HeadShot - NOTE::COMP
	UFUNCTION()
	void HeadShotReaction(float Value);
	UFUNCTION()
	void HeadShotAddImpulse(FVector ImpactPoint);
	UFUNCTION()
	void HeadShotReactionEnd();
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* HeadShotHitSound;
	FTimeline HeadShotTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
    UCurveFloat* HeadShotCurve;
	FTimerHandle HeadShotImpulseDelayTimerHandle;
	float HeadShotBlendValue = 0.0f;

	//Controller
	ABaseEnemyAIController* BaseEnemyAIController;
private:	
	bool IsChasing();
	bool IsGetHitting();
	bool IsAttacking();
	bool IsStrafing();
	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Combat");
	float DestoryTime = 5.f;

	//Behavior Tree
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	uint8 TeamID = 2;
	UPROPERTY(EditAnywhere)
	UBlackboardData* BlackBoard;

	//Parry
	// UFUNCTION(BlueprintCallable)
	// virtual void SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	virtual void PlayStunMontage();
	bool isParryed = false;
	FTimerHandle ParryTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* ParryedMontage;
	

	//Components
	UPROPERTY(VisibleAnywhere)
	UBlackboardComponent* BlackboardComponent;

	//Heal Item Spawn
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AHealth> HealthClass;
	void SpawnHealItem();

};
