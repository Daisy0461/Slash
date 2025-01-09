// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/Enemy.h"
#include "Interfaces/DodgeInterface.h"
#include "Enemy/Warrior/EnemyGuardInterface.h"
#include "WarriorEnemy.generated.h"

class UWarriorWeapon;
class UAnimMontage;
class UBoxComponent;
class AWeapon;
class AWarriorEnemyAIController;
class AEnemyAOEAttack;

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

	//Attack
	virtual void LongRangeAttack_Jump();
	virtual void LongRangeAttack_Spinning();
	virtual void WarriorAOEAttack();
protected:
	UFUNCTION()
	void OnDodgeBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnDodgeBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION(BlueprintCallable)
	virtual void SetDodgeCollision(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	virtual void SetDodgeCharacterIsInEnemyAttackArea();

	//Weapon (Weapon Collision & Parry Collision)
	UFUNCTION(BlueprintCallable)
	void SetWarriorWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	void SetWarriorParryCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType);
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWarriorWeapon();
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWarriorShield();

	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* JumpAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* SpinningAttackMontage;

	//Spin
	bool bIsSpinning = false;
	FTimeline SpinMeshTimeline;
	FRotator OriginRotation;
	UPROPERTY(EditAnywhere, Category = "Spin")
	TSubclassOf<AEnemyAOEAttack> SpinningAOEAttack;
	UPROPERTY(EditDefaultsOnly, Category = "Spin")
	UCurveFloat* SpinCurve;
	
	UFUNCTION()
	void SpinMesh(float Value);
	UFUNCTION(BlueprintCallable)
	void SpinMeshTimelineStart();
	UFUNCTION(BlueprintCallable)
	void SpinAOESpawn();
	virtual void AttackEnd() override;

	//AOE
	UFUNCTION(BlueprintCallable)
	virtual void SpawnWarriorAOE(bool bIsSpinningAttack, bool bIsGroundAttack);
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* WarriorAOEAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Healing")
	TSubclassOf<AEnemyAOEAttack> WarriorAOEClass;
	
	
private:
	AWarriorEnemyAIController* WarriorEnemyAIController;

	UPROPERTY(VisibleAnywhere)
	UWarriorWeapon* WarriorWeapon;
	UFUNCTION(BlueprintCallable)
	virtual void ShortRangeAttack() override;

	bool isEnemyGuarding = false;
	FName GuardingSection = TEXT("EnemyGuarding");
	FName GuardImpactSection = TEXT("EnemyGuardImpact");
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montage")
	UAnimMontage* GuardingAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Animation Montage")
	UAnimMontage* GuardImpactAnimation;

	//Player Dodge Collision
	UPROPERTY(EditAnywhere, Category = "Dodge Collision")
	UBoxComponent* DodgeBox;
	IDodgeInterface* DodgeInterface = nullptr;

};
