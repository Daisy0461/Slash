#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class AShield;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	//Equip
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AWeapon* EquippedWeapon;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Attribute")
	UAttributeComponent* Attributes;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	//Animaion montages
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionName);
	void ChoosePlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	virtual int32 PlayAttackMontage();

	//Attack
	virtual void Attack();
	void PlayHitReactMontage(const FName& SectionName);
	virtual bool CanAttack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	//Hit
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	//Death
	virtual void Die();
	void DisableCapsuleCollision();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category = "Death")
	TArray<FName> DeathMontageSection;
	virtual int32 PlayDeathMontage();


private:
	UPROPERTY(EditAnywhere, Category = "Sounds" )
	USoundBase* HitSound;
	UPROPERTY(EditDefaultsOnly, Category = "VisualEffects")
	UParticleSystem* HitParticles;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSection;
};
