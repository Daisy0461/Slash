#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class AShield;
class UAttributeComponent;
class UAnimMontage;
class UHealthBarComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	//Equip
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedWeapon;

	FORCEINLINE UAttributeComponent* GetAttribute() const {return Attributes; };

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock On")
	AActor* CombatTarget = nullptr;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	virtual void GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter) override;
	virtual void AttackMotionWarp_Implementation() override;
	UPROPERTY(VisibleAnywhere, Category = "Attribute")
	UAttributeComponent* Attributes;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	void AttackMotionWarpAnimNotify();

	//Animaion montages
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);
	void ChoosePlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual int32 PlayAutoAttackMontage();
	virtual int32 PlayMotionWarpAttackMontage();
	void StopAutoAttackMontage();
	void StopMotionWarpAttackMontage();


	//Attack
	float targetHeightOffset = 10.f;
	FVector TargetLocation;
	virtual void Attack();
	void PlayHitReactMontage(const FName& SectionName);
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void SetHitting();
	UFUNCTION(BlueprintCallable)
	virtual void GetHittingEnd();
	//AttackMove
	uint8 bIsAttackingMove;
	float AttackingMoveSpeed;
	float CheckTargetDistance();
	UFUNCTION(BlueprintCallable)
	virtual void AttackingMoveLocating();
	UFUNCTION(BlueprintCallable)
	virtual void AttackRotate();		

	//Motion Wrapping 계산 -> Motion Warp 폐기 - stuck때문에
	UFUNCTION(BlueprintCallable)
	virtual FVector GetTransltaionWarpTarget();
	UFUNCTION(BlueprintCallable)
	virtual FVector GetRotationWarpTarget();
	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance = 75.f;

	//Hit
	virtual void PlayHitSound(const FVector& ImpactPoint);
	virtual void SpawnHitParticle(const FVector& ImpactPoint);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	UFUNCTION(BlueprintCallable)
	virtual void HitMove(FName moveDirection);
	virtual float GetHitMoveValue();
	virtual void SetHitMoveValue(float value);

	//Death
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;
	bool IsAlive();
	virtual void Die();
	void DisableCapsuleCollision();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<FName> DeathMontageSection;
	virtual int32 PlayDeathMontage();
	void DisableMeshCollision();


	//State Check
	FString GetEnumDisplayNameToString(const TCHAR *Enum, int32 EnumValue) const;

protected:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* GuardSound;
	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitParticles;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AutoAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AutoAttackMontageSection;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* MotionWarpAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> MotionWarpAttackMontageSection;

	//공격시 움직이는 거리의 최대값.
	float AttackMoveMaxDistance;

private:
	FVector HitMoveLocation;
	float HitMoveValue;
	float HitMoveSpeed;
	uint8 bIsHitMoving;
	
};