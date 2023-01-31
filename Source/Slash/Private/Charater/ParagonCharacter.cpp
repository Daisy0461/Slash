// Fill out your copyright notice in the Description page of Project Settings.


#include "Charater/ParagonCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AParagonCharacter::AParagonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AParagonCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//BridMappingContext는 BP에서 명시적으로 지정해주며 두번째는 우선순위를 나타낸다.
			Subsystem->AddMappingContext(ParagonIMC, 0);
		}
	}
	
}

void AParagonCharacter::Paragon_Move(const FInputActionValue& value)
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

void AParagonCharacter::Paragon_Look(const FInputActionValue &value)
{
	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AParagonCharacter::Paragon_Jump()
{
}

void AParagonCharacter::Paragon_Equip()
{
}

void AParagonCharacter::Paragon_Attack()
{
}

void AParagonCharacter::Paragon_Dodge()
{
}

void AParagonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



// Called to bind functionality to input
void AParagonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ParagonMovement, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Move);
		EnhancedInputComponent->BindAction(ParagonLook, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Look);
		EnhancedInputComponent->BindAction(ParagonJump, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Jump);
		EnhancedInputComponent->BindAction(ParagonEquip, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Equip);
		EnhancedInputComponent->BindAction(ParagonAttack, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Attack);
		EnhancedInputComponent->BindAction(ParagonDodge, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Dodge);
	}
}

