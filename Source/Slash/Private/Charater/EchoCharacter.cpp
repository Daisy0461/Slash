// Fill out your copyright notice in the Description page of Project Settings.


#include "Charater/EchoCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AEchoCharacter::AEchoCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEchoCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem -> AddMappingContext(EchoIMC, 0);
	}
	
}
void AEchoCharacter::Echo_Move(const FInputActionValue& value)
{
	const FVector2D MoveValue = value.Get<FVector2D>();
	const FVector ForwardVector = GetActorForwardVector();
	AddMovementInput(ForwardVector, MoveValue.Y);

	const FVector RightVector = GetActorRightVector();
	AddMovementInput(RightVector, MoveValue.X);
}

void AEchoCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEchoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(EchoMovement, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Move);
	}
}

