#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;
class UHealthBarComponent;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	FORCEINLINE UAttributeComponent* GetAttribute() const {return Attributes; };
	FORCEINLINE void SetIsInterruptible(bool isInterruptible) {bIsInterruptible = isInterruptible; };
	FORCEINLINE bool GetIsInterruptible() {return bIsInterruptible; };

	//virtual void SetEquippedWeapon(AWeapon* InputWeapon);		//Viking에 없음.
	virtual float GetCharacterHealthPercent();
	virtual void Healing(float HealAmount);

protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock On")
	AActor* CombatTarget = nullptr;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	bool bIsInterruptible = true;
	virtual void GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter) override;
	virtual void GetHeadShot(FVector ImpactPoint) override;
	virtual void GetHitAOEAttack() override;
	UPROPERTY(VisibleAnywhere, Category = "Attribute")
	UAttributeComponent* Attributes;

	//UFUNCTION(BlueprintCallable)
	//virtual void SetWeaponCollision(AWeapon* CollisionWeapon, ECollisionEnabled::Type CollisionEnabled);
	

	//Animaion montages
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);
	void ChoosePlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	// virtual int32 PlayAutoAttackMontage();
	// void StopAutoAttackMontage();


	//Attack
	float targetHeightOffset = 10.f;  
	FVector TargetLocation;
	virtual void Attack();
	void PlayHitReactMontage(const FName& SectionName);
	virtual bool CanAttack(); 
	UFUNCTION(BlueprintCallable)
	virtual void SetHitting();
	UFUNCTION(BlueprintCallable)
	virtual void GetHittingEnd();
	//AttackMove
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* HitReactMontage;

private:
	FVector HitMoveLocation;
	float HitMoveValue;
	float HitMoveSpeed;
	uint8 bIsHitMoving;
	
};