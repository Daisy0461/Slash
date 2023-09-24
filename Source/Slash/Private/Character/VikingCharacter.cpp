// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/VikingCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AttributeComponent.h"
#include "Animation/AnimInstance.h"
#include "Item/Item.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "HUD/VikingHUD.h"
#include "HUD/VikingOverlay.h"
#include "NiagaraFunctionLibrary.h"

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
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetGenerateOverlapEvents(true);
}
// Called to bind functionality to input
void AVikingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(VikingMovement, ETriggerEvent::Triggered, this, &AVikingCharacter::Move);
		EnhancedInputComponent->BindAction(VikingLook, ETriggerEvent::Triggered, this, &AVikingCharacter::Look);
		EnhancedInputComponent->BindAction(VikingJump, ETriggerEvent::Triggered, this, &AVikingCharacter::Jump);
		EnhancedInputComponent->BindAction(VikingEquip, ETriggerEvent::Triggered, this, &AVikingCharacter::Equip);
		EnhancedInputComponent->BindAction(VikingAttack, ETriggerEvent::Triggered, this, &AVikingCharacter::Attack);
		EnhancedInputComponent->BindAction(VikingDodge, ETriggerEvent::Triggered, this, &AVikingCharacter::Dodge);
	}
}

void AVikingCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);

	//공격 중간에 맞았을 때를 위한 상태 변화
	if(Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AVikingCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
    HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
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

	Tags.Add(FName("EngageableTarget"));		//Tag를 더해준다.

	InitializeVikingOverlay(PlayerController);
}
void AVikingCharacter::InitializeVikingOverlay(const APlayerController* PlayerController)
{
	if(PlayerController){
		AVikingHUD* VikingHUD = Cast<AVikingHUD>(PlayerController->GetHUD());
		if(VikingHUD){
			VikingOverlay = VikingHUD->GetVikingOverlay();
			if(VikingOverlay && Attributes){
				VikingOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				VikingOverlay->SetStaminaBarPercent(.7f);
				VikingOverlay->SetSouls(6);
			}
		}
        
    }
}
void AVikingCharacter::SetHUDHealth()
{
	if(VikingOverlay && Attributes){
		VikingOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void AVikingCharacter::SetOverlappingItem(AItem *Item)
{
	OverlappingItem = Item;
}

void AVikingCharacter::PickupHeal(AHealth *Heal)
{
	//변수가 추가적으로 많이 필요하고 Heal은 Health에서 처리하는게 맞다고 판단하서 여긴 비움.
	// if(Attributes){
	// 	//SoundPlay
	// 	Attributes->Heal(10.f);
	// 	SetHUDHealth();
	// }
}
void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVikingCharacter::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	// if(HealthBarWidget){
	// 	HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	// }
}

void AVikingCharacter::Die()
{
	Super::Die();
	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
	
	//죽은 후 Collision 없애기
	// DisableCapsuleCollision();
	// SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	// GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AVikingCharacter::Move(const FInputActionValue& value)
{
	if(ActionState != EActionState::EAS_Unoccupied) return;
	if(!IsAlive()) return;
	
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

void AVikingCharacter::Look(const FInputActionValue &value)
{
	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AVikingCharacter::Jump()
{
	if(IsUnoccupied() && IsAlive())
	{
		Super::Jump(); //이긴한데 삭제할거임.
	}
}

void AVikingCharacter::Equip()		//E를 눌렀을 때 실행된다.
{
	AWeapon* OverlappingWeapon = nullptr; AShield* OverlappingShield = nullptr;

	if(EquippedWeapon == nullptr){
		OverlappingWeapon = Cast<AWeapon>(OverlappingItem);		//이것으로 Overlapping된 것이 AWeapon인지 검사한다.
	}
	
	if(EquippedShield == nullptr){
		OverlappingShield = Cast<AShield>(OverlappingItem);
	}

	//어떤걸 먼저들지 모르기 때문에 1개를 들고있거나 안들고 있을 때로 가정하였다.	Idle 상태에서 두개의 Overlap은 계속된다.
	if(OverlappingWeapon && (CharacterState == ECharacterState::ESC_Origin || CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)){
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);		//무기는 오른손에 장착
		EquippedWeapon = OverlappingWeapon;
		Equip_StateCheck();
	}else if(OverlappingShield && (CharacterState == ECharacterState::ESC_Origin || CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)){
		OverlappingShield->Equip(GetMesh(), FName("LeftHandSocket"));		//방패는 왼손에 장착
		Equip_StateCheck();
		EquippedShield = OverlappingShield;
	}else if(EquippedShield && EquippedWeapon)
	{ 
		EquipAndUnequip();
	}
}

void AVikingCharacter::Equip_StateCheck()
{
	if(CharacterState == ECharacterState::ESC_Origin)
	{	//아무것도 없는 상태에서 무기를 끼면 바꾼다.
		CharacterState = ECharacterState::ESC_EquippedOneHandedWeapon;
	}else if (CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)
	{	//하나를 끼고 있는 상태에서 상태를 바꾸면 2개를 끼고 있는 상태로 바꿔준다.
		CharacterState = ECharacterState::ESC_EquippedTwoHandedWeapon;
	}
}

void AVikingCharacter::EquipAndUnequip()
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

void AVikingCharacter::Attack()
{	
	if(CanAttack()){
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void AVikingCharacter::Dodge()
{
}

void AVikingCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::CanAttack()
{
	return (CharacterState != ECharacterState::ESC_Unequipped && EquippedShield && EquippedWeapon)
	&& IsUnoccupied();
}

void AVikingCharacter::AttachWeaponToBack()
{
	if(EquippedShield && EquippedWeapon){
		EquippedShield -> AttachMeshToSocket(GetMesh(), FName("SpineSocket_Left"));
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("SpineSocket_Right"));
	}
}

void AVikingCharacter::AttachWeaponToHand()
{
	if(EquippedShield && EquippedWeapon){
		EquippedShield -> AttachMeshToSocket(GetMesh(), FName("LeftHandSocket"));
		EquippedWeapon -> AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AVikingCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AVikingCharacter::EndHitReaction()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::IsUnoccupied()
{
    return ActionState == EActionState::EAS_Unoccupied;
}