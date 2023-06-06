// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/CharacterTypes.h"
#include "VikingCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class AShield;

UCLASS()
class SLASH_API AVikingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVikingCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision(ECollisionEnabled::Type CollisionEnable);

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; };
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; };
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState; };

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

private:
	ECharacterState CharacterState = ECharacterState::ESC_Origin;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY()
	AWeapon* EquipedWeapon;
	UPROPERTY()
	AShield* EquipedShield;

	void Viking_Move(const FInputActionValue& value);
	void Viking_Look(const FInputActionValue& value);
	void Viking_Jump();
	void Viking_Equip();
	void Viking_Equip_StateCheck();
	void Viking_EquipAndUnequip();
	void Viking_Attack();
	void Viking_Dodge();

	void Play_Attack_Viking_Montage();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	bool CanAttack();
	UFUNCTION(BlueprintCallable)
	void DisArm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();


	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	//Montage
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* EquipMontage;
};
