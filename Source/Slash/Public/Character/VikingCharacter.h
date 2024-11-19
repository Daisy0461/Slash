// 분해 가능할 것 같은 기능 Equip

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "Interfaces/ParryInterface.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/VikingListener.h"
#include "Character/CharacterTypes.h"
#include "VikingCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UGrappling_Hook;
class UVikingOverlay;
class UNiagaraSystem;
class UAnimMontage;
class UWidget;
class ABow;
class AItem;
class AHealth;
class ATreasure;
class AEnemy;

UCLASS()
class SLASH_API AVikingCharacter : public ABaseCharacter, public IPickupInterface, public IParryInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVikingCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//ABaseCharacter의 IHitInterface에서 override한다.
	UFUNCTION(BlueprintCallable)
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void SetHUDHealth();

	//Parry Interface
	virtual bool ParryCheck() override;
	virtual void RestoreParryTimeDilation() override;
	virtual void SetIsParryDilation(bool ParryDilation) override;
	virtual bool GetIsParryDilation() override;
	bool IsCanParry();
	void SetCustiomTimeDilation(float timeScale);

	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void PickupHeal(AHealth* Heal) override;
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; };
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; };
	FORCEINLINE EActionState GetActionState() const {return ActionState; };
	FRotator GetAimRotation();
	void AddTreasure(ATreasure* Treasure);
	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload);

	//Equip
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWeapon* GetShield() const {return Shield; };
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABow* GetBow() const {return Bow; };
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> EquippedShield;
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<ABow> EquippedBow;
	UPROPERTY()
	AWeapon* Shield;
	UPROPERTY()
	ABow* Bow;
	
	//Camera Lock On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock On")
	float maxTargetingDis;
	bool isTargetLocked = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lock On")
	TArray<AEnemy*> LockOnCandidates;
	UFUNCTION(BlueprintImplementableEvent, Category="Lock On")
	void TargetLockOnEffects();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Die() override;

	//Attack Move
	virtual void AttackRotate() override;

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
	UInputAction* VikingBowAim;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingReleaseBowAim;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingBowShot;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* VikingIMC;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingFirstSkill;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingSecondSkill;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingThirdSkill;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingTargetLock;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* VikingTargetChange;


private:
	ECharacterState CharacterState = ECharacterState::ESC_Origin;
	EActionState ActionState = EActionState::EAS_Unoccupied;
	float RunSpeed = 600.f;
	float GuardWalkSpeed = 220.f;
	
	//Bow
	float BowWalkSpeed = 300.f;
	float BowAimWalkSpeed = 80.f;
	
	//Input
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	virtual void Jump() override;
	void Equip();
	void Attack();
	void Dodge();
	void Guard();
	void ReleaseGuard();
	void FirstSkill();
	void SecondSkill();
	void ThirdSkill();
	void TargetLock_Release();
	void TargetChange();
			

	//Attack
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack() override;
	float CheckTargetDistance();		
	int ComboAttackIndex = 0;
	float AttackX, AttackY;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill1;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill2;
	UPROPERTY(EditDefaultsOnly, Category = "Skill Montage")
	UAnimMontage* Skill3;
	//Bow
	bool isAiming = false;
	void BowAim();
	void BowShot();
	UFUNCTION(BlueprintCallable)
	void BowShotEnd();
	void ReleaseBowAim();


	//Arm
	virtual void EquipWeapon();
	UFUNCTION(BlueprintCallable)
	virtual void AttachBowWeapon();
	UFUNCTION(BlueprintCallable)
	virtual void AttachAxeAndShieldWeapon();
	UFUNCTION(BlueprintCallable)
	virtual void EquipChoose();
	UFUNCTION(BlueprintCallable)
	virtual void FinishEquipping();

	//Equip
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//Montage
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* BowDrawingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* BowShotMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* RollMontage;
	virtual void PlayRollMontage();
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* JumpMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* GuardMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* ParryMontage;
	
	//Component
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere)
	UGrappling_Hook* Grappling_Hook;

	//Hit
	UFUNCTION(BlueprintCallable)
	void EndHitReaction();

	//HUD
	void InitializeVikingOverlay(const APlayerController* PlayerController);
	UPROPERTY()
	UVikingOverlay* VikingOverlay;
	UPROPERTY(EditAnywhere, Instanced, meta = (BindWidget))
	UWidget* BowWidget; 

	//Dodge
	FTimerHandle DodgeCooldownTimerHandle;
	FTimerHandle DodgeInvincibleTimerHandle;
	bool isDodgeCoolTimeEnd = true;
	bool isInvincible = false;
	FVector DodgeTargetLocation;
	UPROPERTY(EditAnywhere)
	float InvincibilityTime = 0.5;
	float DodgeDistance = 300.0f; // 이동 거리
    float DodgeSpeed = 5.0f;     // 보간 속도
	void StartDodgeInvincibilityWindow();
	void ResetInvincibility();
	void ResetDodgeState();
	FVector CalculateDodgeDirection();
	UFUNCTION(BlueprintCallable)
	void EndDodge();
	bool HasEnoughDodgeStamina();

	//Guard
	bool HasEnoughGuardStamina();
	void ChoosGuardState();
	void PlayGuardMontage();
	bool CanGuard(const FVector &ImpactPoint);

	//Parry
	bool CanParry = false;
	void MakeCantParry();
	FTimerHandle ParryTimerHandle;

	bool IsUnoccupied();
	bool IsGuarding();
	bool IsAttacking();
	bool IsSkilling();
};