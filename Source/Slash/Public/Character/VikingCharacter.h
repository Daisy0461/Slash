// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Character/CharacterTypes.h"
#include "VikingCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class AHealth;
class UAnimMontage;
class AShield;
class UVikingOverlay;
class UNiagaraSystem;
class ATreasure;

UCLASS()
class SLASH_API AVikingCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVikingCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//ABaseCharacter의 IHitInterface에서 override한다.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetHUDHealth();

	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void PickupHeal(AHealth* Heal) override;
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; };
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; };
	FORCEINLINE EActionState GetActionState() const {return ActionState; };
	FORCEINLINE EGuardState GetGuardState() const {return GuardState; };
	FORCEINLINE float GetGuardMoveX() const {return GuardMoveX; };
	FORCEINLINE float GetGuardMoveY() const {return GuardMoveY; };
	void AddTreasure(ATreasure* Treasure);
	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload);

	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	AShield* EquippedShield;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Die() override;

	//Input
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingMovement;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingLook;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingJump;
	UPROPERTY(EditAnywhere, Category = "Input") 
	UInputAction* VikingEquip;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingAttack;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingDodge;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingGuard;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingReleaseGuard;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* VikingIMC;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingFirstSkill;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingSecondSkill;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingThirdSkill;

private:
	ECharacterState CharacterState = ECharacterState::ESC_Origin;
	EActionState ActionState = EActionState::EAS_Unoccupied;
	EGuardState GuardState = EGuardState::EGS_NotGuarding;
	float GuardMoveX;
	float GuardMoveY;
	float RunSpeed = 600.f;
	float GuardWalkSpeed = 220.f;
	
	//Input
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void GuardingLook();
	void ReleaseGuardingLook();
	virtual void Jump() override;
	void Equip();
	void Equip_StateCheck();
	void EquipAndUnequip();
	void Attack();
	void Dodge();
	void Guard();
	void ReleaseGuard();
	void FirstSkill();
	void SecondSkill();
	void ThirdSkill();

	//Attack
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	int ComboAttackIndex = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill1;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill2;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill3;

	//Arm
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void SetShieldCollision(ECollisionEnabled::Type CollisionType);

	//Equip
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//Montage
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AutoAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* RollMontage;
	void PlayRollMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* JumpMontage;
	void PlayJumpMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* GuardMontage;
	
	//Component
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	//Hit
	UFUNCTION(BlueprintCallable)
	void EndHitReaction();

	//HUD
	void InitializeVikingOverlay(const APlayerController* PlayerController);
	UPROPERTY()
	UVikingOverlay* VikingOverlay;

	//Dodge
	UFUNCTION(BlueprintCallable)
	void EndDodge();
	bool HasEnoughDodgeStamina();

	//Guard
	bool HasEnoughGuardStamina();
	void ChoosGuardState();
	void PlayGuardMontage();
	bool CanGuard(const FVector &ImpactPoint);

	bool IsUnoccupied();
	bool IsGuarding();
	bool IsAttacking();
	bool IsSkilling();
};