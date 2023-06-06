// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/VikingCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Item/Item.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "Components/BoxComponent.h"

AVikingCharacter::AVikingCharacter()
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

void AVikingCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//BridMappingContext는 BP에서 명시적으로 지정해주며 두번째는 우선순위를 나타낸다.
			Subsystem->AddMappingContext(VikingIMC, 0);
		}
	}
}

void AVikingCharacter::EnableWeaponCollision(ECollisionEnabled::Type CollisionEnable)
{
	if(EquipedWeapon && EquipedWeapon->GetWeaponBox())
	{
		EquipedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnable);
		EquipedWeapon->IgnoreActors.Empty();
	}
}

void AVikingCharacter::Viking_Move(const FInputActionValue& value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;
	
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

void AVikingCharacter::Viking_Look(const FInputActionValue &value)
{
	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AVikingCharacter::Viking_Jump()
{

}

void AVikingCharacter::Viking_Equip_StateCheck()
{
	if(CharacterState == ECharacterState::ESC_Origin)
	{	//아무것도 없는 상태에서 무기를 끼면 바꾼다.
		CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
	}else if (CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)
	{	//하나를 끼고 있는 상태에서 상태를 바꾸면 2개를 끼고 있는 상태로 바꿔준다.
		CharacterState = ECharacterState::ESC_EquippedTwoHandedWeapon;
	}
}

void AVikingCharacter::Viking_EquipAndUnequip()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage){
		AnimInstance->Montage_Play(EquipMontage);
		FName SectionName = FName();

		if(CharacterState == ECharacterState::ESC_Unequipped)
		{	//아무것도 없는 상태에서 무기를 끼면 바꾼다.
			CharacterState = ECharacterState::ESC_EquippedTwoHandedWeapon;
			PlayAnimMontage(EquipMontage, 1, FName("Equip"));
			ActionState = EActionState::EAS_Equipping;
		}else if (CharacterState == ECharacterState::ESC_EquippedTwoHandedWeapon)
		{
			CharacterState = ECharacterState::ESC_Unequipped;
			PlayAnimMontage(EquipMontage, 1, FName("Unequip"));
			ActionState = EActionState::EAS_Equipping;
		}
	}
}

void AVikingCharacter::Viking_Equip()		//E를 눌렀을 때 실행된다.
{
	AWeapon* OverlappingWeapon = nullptr; AShield* OverlappingShield = nullptr;

	if(EquipedWeapon == nullptr){
		OverlappingWeapon = Cast<AWeapon>(OverlappingItem);		//이것으로 Overlapping된 것이 AWeapon인지 검사한다.
	}
	
	if(EquipedShield == nullptr){
		OverlappingShield = Cast<AShield>(OverlappingItem);
	}

	//어떤걸 먼저들지 모르기 때문에 1개를 들고있거나 안들고 있을 때로 가정하였다.	Idle 상태에서 두개의 Overlap은 계속된다.
	if(OverlappingWeapon && (CharacterState == ECharacterState::ESC_Origin || CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)){
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));		//무기는 오른손에 장착
		OverlappingWeapon->SetOwner(this);
		OverlappingWeapon->SetInstigator(this);
		EquipedWeapon = OverlappingWeapon;
		Viking_Equip_StateCheck();
	}else if(OverlappingShield && (CharacterState == ECharacterState::ESC_Origin || CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)){
		OverlappingShield->Equip(GetMesh(), FName("LeftHandSocket"));		//방패는 왼손에 장착
		OverlappingShield->SetOwner(this);
		OverlappingShield->SetInstigator(this);
		Viking_Equip_StateCheck();
		EquipedShield = OverlappingShield;
	}else if(EquipedShield && EquipedWeapon)
	{ 
		Viking_EquipAndUnequip();
	}
}

void AVikingCharacter::Viking_Attack()
{	
	if(CanAttack()){
		Play_Attack_Viking_Montage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AVikingCharacter::Viking_Dodge()
{
}

void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVikingCharacter::Play_Attack_Viking_Montage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && AttackMontage){
		AnimInstance -> Montage_Play(AttackMontage);
		FName SectionName = FName();
		const int32 Selection = FMath::RandRange(0, 1);
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack_1");
			break;
		case 1:
			SectionName = FName("Attack_2");
			break;
		default:
			SectionName = FName("Attack_1");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}
void AVikingCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::CanAttack()
{
	return (CharacterState != ECharacterState::ESC_Unequipped && EquipedShield && EquipedWeapon)
	&& ActionState == EActionState::EAS_Unoccupied;
}

void AVikingCharacter::DisArm()
{
	if(EquipedShield && EquipedWeapon){
		EquipedShield -> AttachMeshToSocket(GetMesh(), FName("SpineSocket_Left"));
		EquipedWeapon -> AttachMeshToSocket(GetMesh(), FName("SpineSocket_Right"));
	}
}

void AVikingCharacter::Arm()
{
	if(EquipedShield && EquipedWeapon){
		EquipedShield -> AttachMeshToSocket(GetMesh(), FName("LeftHandSocket"));
		EquipedWeapon -> AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}
void AVikingCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

// Called to bind functionality to input
void AVikingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(VikingMovement, ETriggerEvent::Triggered, this, &AVikingCharacter::Viking_Move);
		EnhancedInputComponent->BindAction(VikingLook, ETriggerEvent::Triggered, this, &AVikingCharacter::Viking_Look);
		EnhancedInputComponent->BindAction(VikingJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(VikingEquip, ETriggerEvent::Triggered, this, &AVikingCharacter::Viking_Equip);
		EnhancedInputComponent->BindAction(VikingAttack, ETriggerEvent::Triggered, this, &AVikingCharacter::Viking_Attack);
		EnhancedInputComponent->BindAction(VikingDodge, ETriggerEvent::Triggered, this, &AVikingCharacter::Viking_Dodge);
	}
}
