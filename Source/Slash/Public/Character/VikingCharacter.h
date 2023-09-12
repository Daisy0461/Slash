// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "VikingCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AShield;
class UVikingOverlay;

UCLASS()
class SLASH_API AVikingCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVikingCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//ABaseCharacter의 IHitInterface에서 override한다.
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; };
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; };
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; };

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
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
	UInputMappingContext* VikingIMC;

private:
	ECharacterState CharacterState = ECharacterState::ESC_Origin;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	virtual void Jump() override;
	void Equip();
	void Equip_StateCheck();
	void EquipAndUnequip();
	void Attack();
	void Dodge();

	//Attack
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	//Arm
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	//Equip
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	UPROPERTY()
	AShield* EquippedShield;

	//Montage
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;

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
	void SetHUDHealth();
	UPROPERTY()
	UVikingOverlay* VikingOverlay;

	bool IsUnoccupied();
};
