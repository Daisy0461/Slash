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

	const FRotator Rotation = Controller->GetControlRotation();		//Control의 Rotaion을 들고온다. Control은 Position은 없지만 Rotation은 있다.
	const FRotator YawRoation(0.f, Rotation.Yaw, 0.f);		//Controller의 Yaw값으로 YawRotaion을 초기화한다.

	//Controller의 Rotation에 알맞게 앞쪽 방향을 찾는다.
	//아래의 결과는 Controller가 가리키는 곳에 대한 ForwardVector를 얻는다.
	const FVector ForwardDirection = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MoveValue.Y);
	//아래의 방법은 Controller의 rightVector를 얻는다. 
	const FVector RightDirection = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MoveValue.X);
}
void AEchoCharacter::Echo_Look(const FInputActionValue &value)
{
	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}
void AEchoCharacter::Echo_Jump()
{
}
void AEchoCharacter::Echo_Equip()
{
}
void AEchoCharacter::Echo_Attack()
{
}
void AEchoCharacter::Echo_Dodge()
{
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
		EnhancedInputComponent->BindAction(EchoLook, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Look);
		EnhancedInputComponent->BindAction(EchoJump, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Jump);
		EnhancedInputComponent->BindAction(EchoEquip, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Equip);
		EnhancedInputComponent->BindAction(EchoAttack, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Attack);
		EnhancedInputComponent->BindAction(EchoDodge, ETriggerEvent::Triggered, this, &AEchoCharacter::Echo_Dodge);
	}
}

