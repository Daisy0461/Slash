// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ParagonCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Item.h"
#include "Item/Weapons/Weapon.h"


AParagonCharacter::AParagonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//아래의 기능은 BP에서도 설정 할 수 있고 기능의 역할은
	//Controller를 회전시켰을 때 자연스럽게 캐릭터가 Controller의 앞쪽 방향으로 바라보게 하는 것이다.
	//바라보게하는 회전 속도 등 다양한 값을 바꿀 수 있다. 
	//-> GetCharacterMovement()->RotationRate = FRotator (0.f, 400.f, 0.f); Yaw방향으로 회전하는 속도를 400.f로 맞추는 것이다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
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

void AParagonCharacter::Paragon_Equip()		//E를 눌렀을 때 실행된다.
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if(OverlappingWeapon){
		OverlappingWeapon->Equip(this->GetMesh(), FName("RightHandSocket"));\
		CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
	}
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
		EnhancedInputComponent->BindAction(ParagonJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(ParagonEquip, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Equip);
		EnhancedInputComponent->BindAction(ParagonAttack, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Attack);
		EnhancedInputComponent->BindAction(ParagonDodge, ETriggerEvent::Triggered, this, &AParagonCharacter::Paragon_Dodge);
	}
}
