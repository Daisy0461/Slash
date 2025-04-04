// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Components/TimelineComponent.h"
#include "Enemy/EnemyInterface.h"
#include "Enemy/EnemyAttacks/EnemyAutoAttackInterface.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackInterface.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackInterface.h"
#include "Enemy/EnemyAttacks/EnemyThrowWeaponAttackInterface.h"
#include "Enemy/EnemyAttacks/EnemyTeleportInterface.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackEnum.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"
#include "Enemy/EnemyEnum/EnemyState.h"
#include "Enemy/EnemyEnum/EnemyMovementEnum.h"
#include "ProceduralMeshComponent.h"
#include "Enemy.generated.h"

class UHealthBarComponent; 
class UEnemyMoveComponent;
class UAnimMontage;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Damage;
class UProceduralMeshComponent;
class UEnemyAutoAttackComponent;
class UEnemyThrowWeaponAttackComponent;
class UCurveFloat;
class AVikingGameState;
class UBehaviorTree;
class UBlackboardComponent;
class UBlackboardData;
class UHealthBar;
class UMetaSoundSource;
class AHealth;
class AWeapon;
class ABaseEnemyAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyHit);

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter, public IEnemyInterface, 
public IEnemyAutoAttackInterface, public IEnemyAOEAttackInterface, public IEnemyFireBallAttackInterface, public IEnemyThrowWeaponAttackInterface, public IEnemyTeleportInterface
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

	//Damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	FORCEINLINE void SetReduceDamagePercent(float ReducePercent) {ReduceDamagePercent = ReducePercent; };

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Passive;
	
	EEnemyState GetEnemyState();
	FORCEINLINE void SetEnemyState(EEnemyState State) {EnemyState = State;}
	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree();
	FORCEINLINE uint8 GetTeamID() {return TeamID; }
	UBlackboardComponent* GetBlackboardComponent() const;
	float GetAttackRadius() const;
	FORCEINLINE void SetAttackRadius(float Radius) {AttackRadius = Radius;};
	float GetDefendRadius() const;
	FORCEINLINE void SetDefendRadius(float Radius) {DefendRadius = Radius;};

	//Attack
	AVikingGameState* VikingGameState;
	FORCEINLINE ABaseEnemyAIController* GetBaseEnemyAIController() {return BaseEnemyAIController; };
	UAnimInstance* GetEnemyAnimInstance() const;
	UFUNCTION(BlueprintCallable)
	virtual void EnemyAutoAttack(bool bIsRandom, int32 SectionNum) override;
	UFUNCTION(BlueprintCallable)
	virtual void EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType) override;
	UFUNCTION(BlueprintCallable)
	virtual UEnemyAOEAttackComponent* GetEnemyAOEAttack();
	UFUNCTION(BlueprintCallable)
	virtual UEnemyFireBallAttackComponent* GetEnemyFireBall();
	UFUNCTION(BlueprintCallable)
	virtual void EnemyFireBallAttack(EEnemyFireBallEnum FireBallType) override;
	UFUNCTION(BlueprintCallable)
	virtual UEnemyThrowWeaponAttackComponent* GetEnemyThrowWeaponAttack();
	virtual void EnemyThrowWeaponAttack() override;
	virtual void EnemyTeleport(EEnemyTeleportEnum TeleportFunctionType) override;
	virtual void SetAIAttackFinishDelegate(const FAIEnemyAttackFinished& InOnAttackFinished) override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UPROPERTY()
	FOnEnemyDeath OnEnemyDeath;
	UPROPERTY()
	FOnEnemyHit OnEnemyHit;


	//Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	EEnemyMovementSpeed CurrentMovementSpeed = EEnemyMovementSpeed::EEMS_Idle;
    virtual void SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed);
    EEnemyMovementSpeed GetMovementSpeedEnum() const;
	
	//Patroll NOTE::COMP
	UPROPERTY(EditAnywhere)
	AActor* PatrollSpline;
	UFUNCTION(BlueprintCallable)
	virtual AActor* GetPatrolRoute() const override;

	//Weapon
	FORCEINLINE float GetDestoryTime() {return DestoryTime; }
	UFUNCTION()
	FVector GetGroundLocation(AActor* Actor);

	//Health
	void HideHealthBar();
	void ShowHealthBar();
	virtual UHealthBar* GetBossHealthBar();
	virtual void Healing(float HealAmount) override;
protected:
	UPROPERTY(VisibleAnywhere)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditAnywhere, Category = "Combat");
	float AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = "Combat");
	float DefendRadius = 500.f;

	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual void SetHitting() override; 
	virtual void GetHittingEnd() override;

	//EnmeyDie
	virtual void BreakSkeletalBone(FVector ImpactPoint, FVector ImpactNormal, FName BreakBoneName) override;
	FName SelectDieAnimation();

	virtual bool CanAttack() override;
	//Damage
	virtual void HandleDamage(float DamageAmount) override;
	float ReduceDamagePercent = 0.f;

	
	UPROPERTY(EditDefaultsOnly)
	UHealthBar* BossHealthBar;

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

	//movementSpeed
	float IdleMovementSpeed = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "MovementSpeed")
	float WalkMovementSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "MovementSpeed")
	float JoggingMovementSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "MovementSpeed")
	float SprintMovementSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = "MovementSpeed")
	float TeleportMovementSpeed = 600.f;

	//Slice
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProcMeshComponent;
	UPROPERTY(EditAnywhere, Category = "Slice")
	FName TargetBoneName = "spine_02";
	UPROPERTY(EditAnywhere, Category = "Slice")
	FName ProceduralMeshAttachSocketName = "ProcedrualMesh_Attach";
	UPROPERTY(EditAnywhere, Category = "Slice")
	FName OtherHalfMeshAttachSocketName = "OtherHalfMesh_Attach";
	UPROPERTY(EditAnywhere, Category = "Slice")
	float CreateProceduralMeshDistance = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slice")
    UMaterialInterface* CapMaterial;
	int32 NumVertices = 0;
	TMap<int32, int32> VertexIndexMap;
	TArray<FVector> FilteredVerticesArray;
	TArray<int32> Indices;
    TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
    TArray<FVector2D> UV;
	TArray<FColor> VertexColors;
    TArray<FColor> Colors;
	
	void SelectVertices(int32 LODIndex);
	void ApplyVertexAlphaToSkeletalMesh();
	void CopySkeletalMeshToProcedural(int32 LODIndex);
	void SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf);
private:	
	bool IsChasing();
	bool IsGetHitting();
	bool IsAttacking();
	bool IsStrafing();
	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Combat");
	float DestoryTime = 5.f;

	//Behavior Tree ?? 필요?
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	uint8 TeamID = 2;

	//Parry
	// UFUNCTION(BlueprintCallable)
	// virtual void SetParryBoxCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	virtual void PlayStunMontage();
	bool isParryed = false;
	bool bIsPlayStunMontage = false;
	FTimerHandle ParryTimer;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* ParryedMontage;

	//Heal Item Spawn
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AHealth> HealthClass;
	void SpawnHealItem();
 
};
