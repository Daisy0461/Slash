

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EchoCharacter.generated.h"


class UInputAction;
class UInputMappingContext;

UCLASS()
class SLASH_API AEchoCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEchoCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoMovement;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoLook;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoJump;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoEquip;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoAttack;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EchoDodge;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* EchoIMC;

private:
	void Echo_Move(const FInputActionValue& value);
	void Echo_Look(const FInputActionValue& value);
	void Echo_Jump();
	void Echo_Equip();
	void Echo_Attack();
	void Echo_Dodge();

};
