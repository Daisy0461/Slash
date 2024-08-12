// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/VikingCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Animation/AnimInstance.h"
#include "Item/Item.h"
#include "Item/Treasure.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "Enemy/Enemy.h"
#include "HUD/VikingHUD.h"
#include "HUD/VikingOverlay.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

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

	//Target Lock Init
	isTargetLocked = false;
	//targetHeightOffset = 10.f;
	maxTargetingDis = 2000.f;

	//Attack Move Init
	bIsAttackingMove = false;
	AttackingMoveSpeed = 5.0f;

	
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
		EnhancedInputComponent->BindAction(VikingJump, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(VikingEquip, ETriggerEvent::Triggered, this, &AVikingCharacter::Equip);
		EnhancedInputComponent->BindAction(VikingAttack, ETriggerEvent::Triggered, this, &AVikingCharacter::Attack);
		EnhancedInputComponent->BindAction(VikingDodge, ETriggerEvent::Triggered, this, &AVikingCharacter::Dodge);
		EnhancedInputComponent->BindAction(VikingGuard, ETriggerEvent::Triggered, this, &AVikingCharacter::Guard);
		EnhancedInputComponent->BindAction(VikingReleaseGuard, ETriggerEvent::Triggered, this, &AVikingCharacter::ReleaseGuard);
		EnhancedInputComponent->BindAction(VikingFirstSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::FirstSkill);
		EnhancedInputComponent->BindAction(VikingSecondSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::SecondSkill);
		EnhancedInputComponent->BindAction(VikingThirdSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::ThirdSkill);
		EnhancedInputComponent->BindAction(VikingTargetLock, ETriggerEvent::Triggered, this, &AVikingCharacter::TargetLock_Release);
		EnhancedInputComponent->BindAction(VikingTargetChange, ETriggerEvent::Triggered, this, &AVikingCharacter::TargetChange);
	}
}

void AVikingCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(IsGuarding() && CanGuard(Hitter->GetActorLocation()) && Attributes){
		//
		if(CanParry && ParryMontage){
			ChoosePlayMontageSection(ParryMontage, TEXT("Parry"));
		}else{
			PlayGuardMontage();		
			Attributes->Heal(7.f);  //Guard시 TakeDamage를 if문으로 돌릴 방법을 찾지 못해서 일단 Heal을 하는 방식으로 적용			
		}

		EquippedShield->SpawnShieldParticle();
		EquippedShield->PlayShieldSound(ImpactPoint);

	}else{
		//Guard 방향이 맞지 않을 때
		Super::GetHit_Implementation(ImpactPoint, Hitter);
		
		PlayHitSound(ImpactPoint);
		SpawnHitParticle(ImpactPoint);
	}

	//공격 중간에 맞았을 때를 위한 상태 변화
	if(!IsGuarding() && Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
		ComboAttackIndex = 0;		//공격중에 클릭하고 나서 맞았을 경우에 ComboAttackIndex를 초기화해준다.
	}
	SetWeaponCollision(ECollisionEnabled::NoCollision);
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

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance){
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AVikingCharacter::HandleOnMontageNotifyBegin);
	}
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
				VikingOverlay->SetTreasures(0);
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
void AVikingCharacter::AddTreasure(ATreasure* Treasure)
{
	if(Attributes && VikingOverlay){
		Attributes->AddTreasure(Treasure->GetTreasure());
		VikingOverlay->SetTreasures(Attributes->GetTreasure());
	}
}

void AVikingCharacter::HandleOnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload &BranchingPayload)
{
	ComboAttackIndex--;

	if(ComboAttackIndex < 0){
		ActionState = EActionState::EAS_Unoccupied;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance){
			StopAutoAttackMontage();
		}
	}
}

void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Attributes && VikingOverlay){
		Attributes->StaminaRegen(DeltaTime);
		VikingOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	//Target Lock
	if(isTargetLocked){
		AEnemy* LockedOnActor = Cast<AEnemy>(CombatTarget);
		if(LockedOnActor){
			if(LockedOnActor->GetEnemyState() == EEnemyState::EES_Dead){
				//UE_LOG(LogTemp, Display, TEXT("Dead"));
				TargetLock_Release();
				return;
			}

			if(GetDistanceTo(LockedOnActor) <= maxTargetingDis){
				FRotator lookRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedOnActor->GetActorLocation());
				lookRotation.Pitch -= targetHeightOffset;
				GetController()->SetControlRotation(lookRotation);
				TargetLockOnEffects();
			}else{
				TargetLock_Release();
			}
		}
	}

	
}

void AVikingCharacter::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
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
	if(!IsAlive()) return; 

	const FVector2D MoveValue = value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();		//Control의 Rotaion을 들고온다. Control은 Position은 없지만 Rotation은 있다.
	const FRotator YawRoation(0.f, Rotation.Yaw, 0.f);		//Controller의 Yaw값으로 YawRotaion을 초기화한다.
	//아래의 결과는 Controller가 가리키는 곳에 대한 ForwardVector를 얻는다.
	const FVector ForwardDirection = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::X);
	//아래의 방법은 Controller의 rightVector를 얻는다. 
	const FVector RightDirection = FRotationMatrix(YawRoation).GetUnitAxis(EAxis::Y);

	if(IsUnoccupied()){		//Gurad가 아닌 일반적인 상태일때의 Move
		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);
	}else if(IsGuarding()){		//Guard중 일때의 Move	
		GuardMoveX = MoveValue.X; GuardMoveY = MoveValue.Y;
		ChoosGuardState();
		AddMovementInput(ForwardDirection, MoveValue.Y);
		AddMovementInput(RightDirection, MoveValue.X);	
	}
}

void AVikingCharacter::Look(const FInputActionValue &value)
{
	//카메라가 회전하는 작업.
	if(isTargetLocked) return;

	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AVikingCharacter::GuardingLook()
{
	bUseControllerRotationYaw = true;
}

void AVikingCharacter::ReleaseGuardingLook()
{
	bUseControllerRotationYaw = false;
}

void AVikingCharacter::Jump()
{
	//일시적 삭제
	if((IsUnoccupied() || IsGuarding()) && IsAlive())
	{
		Super::Jump();
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
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"),this, this);		//무기는 오른손에 장착
		EquippedWeapon = OverlappingWeapon;
		Equip_StateCheck();
	}else if(OverlappingShield && (CharacterState == ECharacterState::ESC_Origin || CharacterState == ECharacterState::ESC_EquippedOneHandedWeapon)){
		OverlappingShield->Equip(GetMesh(), FName("LeftHandSocket"),this, this);		//방패는 왼손에 장착
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
	if(IsAttacking())
	{
		ComboAttackIndex = 1;
	}

	if(CanAttack()){
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance){
			PlayAutoAttackMontage();
		}

		AttackRotate();
		
		ActionState = EActionState::EAS_Attacking;
		//AttackMotionWarpAnimNotify();
	}
}

void AVikingCharacter::AttackRotate()
{
	Super::AttackRotate();

	if(isTargetLocked && CombatTarget){
		const FVector CuurentActorLocation = GetActorLocation();
		const FVector LockedOnActorLocation = CombatTarget->GetActorLocation();
		FVector Direction = LockedOnActorLocation - CuurentActorLocation;
		Direction.Z = 0.0f;		//Z축 회전 무시

		FRotator ActorRotate = FRotationMatrix::MakeFromX(Direction).Rotator();
		SetActorRotation(ActorRotate);
	}
}

void AVikingCharacter::Dodge()
{
	if(!(IsUnoccupied() || IsGuarding() || IsSkilling()) ||
	!HasEnoughDodgeStamina() && VikingOverlay) return;

	PlayRollMontage();
	Attributes->UseStamina(Attributes->GetDodgeCost());
	VikingOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());

	ActionState = EActionState::EAS_Dodge;
}

void AVikingCharacter::Guard()
{
	if(!(IsUnoccupied() || IsGuarding()) || 
	!HasEnoughDodgeStamina() && VikingOverlay) return;

	ActionState = EActionState::EAS_Guard;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = GuardWalkSpeed;
	Attributes->UseStamina(Attributes->GetGuardCost());	
	GuardingLook();

	CanParry = true;
	float HitStopTime = 0.3f;
	//Parry Check -> Guard하고 시간을 잴꺼임
	if(GetWorld()){
		GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &AVikingCharacter::MakeCantParry, HitStopTime, false);
	}

}

void AVikingCharacter::ReleaseGuard()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReleaseGuard"));
	if(!IsGuarding()) return;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	ActionState = EActionState::EAS_Unoccupied;
	GuardState = EGuardState::EGS_NotGuarding;
	ReleaseGuardingLook();

	CanParry = false; 
}

void AVikingCharacter::MakeCantParry()
{
	CanParry = false;
}

bool AVikingCharacter::IsCanParry()
{
	//UE_LOG(LogTemp, Display, TEXT("(In Viking Parry Check)"));
	return CanParry;
}

void AVikingCharacter::SetCustiomTimeDilation(float timeScale)
{
	//UE_LOG(LogTemp, Display, TEXT("Viking CustomTimeDilation : %f"), timeScale);
	CustomTimeDilation = timeScale;
}

void AVikingCharacter::FirstSkill()
{
	if(Attributes && Skill1 && !IsSkilling()){
		Attributes->UseStamina(Attributes->GetFirstSkillCost());
		ActionState = EActionState::EAS_Skilling;
		PlayAnimMontage(Skill1);
	}
}

void AVikingCharacter::SecondSkill()
{
	if(Attributes && Skill2 && !IsSkilling()){
		Attributes->UseStamina(Attributes->GetSecondSkillCost());
		ActionState = EActionState::EAS_Skilling;
		PlayAnimMontage(Skill2);
	}
}

void AVikingCharacter::ThirdSkill()
{
	if(Attributes && Skill3 && !IsSkilling()){
		Attributes->UseStamina(Attributes->GetThirdSkillCost());
		ActionState = EActionState::EAS_Skilling;
		PlayAnimMontage(Skill3);
	}
}

void AVikingCharacter::TargetLock_Release()
{
	if(isTargetLocked){
		//release 구현
		isTargetLocked = false;
		CombatTarget = nullptr;

		TargetLockOnEffects();

		bUseControllerRotationYaw = false;
	}else{
		//TargetLock 구현
		//문제?
		UE_LOG(LogTemp, Display, TEXT("In Target Lock & Not Target Locked if"));
		UE_LOG(LogTemp, Display, TEXT("LockOnCandidates Num : %d"), LockOnCandidates.Num());
		if(LockOnCandidates.Num()> 0){
			AEnemy* closestEnemy = LockOnCandidates[0];

			for(int i=1; i<LockOnCandidates.Num(); i++){
				if(GetDistanceTo(LockOnCandidates[i]) < GetDistanceTo(closestEnemy)){
					closestEnemy = LockOnCandidates[i];
				}
			}

			CombatTarget = closestEnemy;

			if(CombatTarget){
				TargetLockOnEffects();
				isTargetLocked = true;
				//GetCharacterMovement()->bOrientRotationToMovement = false;
			}

		}
	}
}

void AVikingCharacter::TargetChange()
{
	if(!isTargetLocked) return;

	//Change에서 문제가 생길 수도? AActor가 AEnemy를 담을 수 있나?
	for(int i=0; i<LockOnCandidates.Num(); i++){
		if(CombatTarget == LockOnCandidates[i]){
			if(i >= LockOnCandidates.Num() - 1){
				//UE_LOG(LogTemp, Display, TEXT("Target is 0"));
				CombatTarget = LockOnCandidates[0];
				TargetLockOnEffects();
				break;		//break가 없으면 for문을 돌아서 결국 0으로 온다.
			}else{
				//UE_LOG(LogTemp, Display, TEXT("Target is not 0"));
				CombatTarget = LockOnCandidates[i+1];
				TargetLockOnEffects();
				break;
			}
		}
	}
}

float AVikingCharacter::CheckTargetDistance()
{
	//Target이 Lock On 되어있을 때만 수행
	float Distance = 180.f;
	if(CombatTarget){
		Distance = GetDistanceTo(CombatTarget);
		//UE_LOG(LogTemp, Display, TEXT("Distance : %f"), Distance);

		if(Distance < 120.f){
			Distance = 120.f;
		}else if(Distance > 180.f){
			Distance = 180.f;
		}
	}

	return Distance;
}


void AVikingCharacter::AttackEnd()
{
	UE_LOG(LogTemp, Display, TEXT("AttackEnd"));
	ActionState = EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::CanAttack()
{
	return (CharacterState != ECharacterState::ESC_Unequipped && EquippedShield && EquippedWeapon)
	&& IsUnoccupied() && !IsSkilling();
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

void AVikingCharacter::SetShieldCollision(ECollisionEnabled::Type CollisionType)
{
	if(EquippedShield && EquippedShield->GetShieldBox())
	{	
		EquippedShield->IgnoreActors.Empty();
		EquippedShield->IgnoreActors.Add(GetOwner());

		EquippedShield->GetShieldBox()->SetCollisionEnabled(CollisionType);
	}
}


void AVikingCharacter::PlayRollMontage(){
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && RollMontage){
		AnimInstance->Montage_Play(RollMontage);
		AnimInstance->Montage_JumpToSection(FName("Default"), RollMontage);
	}
}

void AVikingCharacter::PlayJumpMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && JumpMontage){
		AnimInstance->Montage_Play(JumpMontage);
		AnimInstance->Montage_JumpToSection(FName("Default"), JumpMontage);
	}
}
void AVikingCharacter::EndHitReaction() 
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AVikingCharacter::EndDodge()
{
    ActionState = EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::HasEnoughDodgeStamina()
{
    return Attributes && (Attributes->GetStamina() >= Attributes->GetDodgeCost());
}

bool AVikingCharacter::HasEnoughGuardStamina()
{
    return Attributes && (Attributes->GetStamina() >= Attributes->GetGuardCost());
}

void AVikingCharacter::ChoosGuardState()
{
	if(GuardMoveX == 0 && GuardMoveY == 1){GuardState = EGuardState::EGS_Front;}
	else if (GuardMoveX == -1 && GuardMoveY == 1){GuardState = EGuardState::EGS_FrontL45;}
	else if (GuardMoveX == 1 && GuardMoveY == 1){GuardState = EGuardState::EGS_FrontR45;}
	else if (GuardMoveX == -1 && GuardMoveY == 0){GuardState = EGuardState::EGS_Left;}
	else if (GuardMoveX == -1 && GuardMoveY == -1){GuardState = EGuardState::EGS_BackL45;}
	else if (GuardMoveX == 0 && GuardMoveY == -1){GuardState = EGuardState::EGS_Back;}
	else if (GuardMoveX == 1 && GuardMoveY == -1){GuardState = EGuardState::EGS_BackR45;}
	else if (GuardMoveX == 1 && GuardMoveY == 0){GuardState = EGuardState::EGS_Right;}
}

void AVikingCharacter::PlayGuardMontage()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && GuardMontage){
		AnimInstance->Montage_Play(GuardMontage);
		AnimInstance->Montage_JumpToSection(FName("Default"), GuardMontage);
	}
}

bool AVikingCharacter::CanGuard(const FVector &ImpactPoint)
{
    const FVector EnemyForward = GetActorForwardVector();
	const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	//Forward * ToHit = |Forward| * |ToHit| * cos(theta) = cos(theta)
	const double CosTheta = FVector::DotProduct(EnemyForward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(EnemyForward, ToHit);
	if(CrossProduct.Z < 0){		//-일떄만 Theta값 -값으로 바꿔주면 된다.
		Theta = -1.f * Theta;
	}

	if(Theta < 60.f && -60.f <=Theta){
		//Front
		return true;
	}else{
		return false;
	}
}

bool AVikingCharacter::IsUnoccupied()
{
    return ActionState == EActionState::EAS_Unoccupied;
}

bool AVikingCharacter::IsGuarding()
{
    return ActionState == EActionState::EAS_Guard;
}

bool AVikingCharacter::IsAttacking()
{
    return ActionState == EActionState::EAS_Attacking;
}

bool AVikingCharacter::IsSkilling()
{
    return ActionState == EActionState::EAS_Skilling;
}