// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EchoMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* EchoIMC;

private:
	void Echo_Move(const FInputActionValue& value);

};
