// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/CharacterTypes.h"
#include "ParagonCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class AItem;

UCLASS()
class SLASH_API AParagonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AParagonCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; };
	FORCEINLINE AItem* GetOverlappingItem() const { return OverlappingItem; };
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState};

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonMovement;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonLook;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonJump;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonEquip;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonAttack;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ParagonDodge;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* ParagonIMC;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

private:
	ECharacterState CharacterState = ECharacterState::ESC_Unequipped;

	void Paragon_Move(const FInputActionValue& value);
	void Paragon_Look(const FInputActionValue& value);
	void Paragon_Jump();
	void Paragon_Equip();
	void Paragon_Attack();
	void Paragon_Dodge();

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
};