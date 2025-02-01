// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/VikingCharacter.h"
#include "Character/Grappling_Hook.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimInstance.h"
#include "Item/Item.h"
#include "Item/Treasure.h"
//VikingWeapon으로 두 h파일 대체
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Bow.h"
#include "Character/VikingWeapon.h"
#include "Enemy/Enemy.h"

#include "HUD/VikingHUD.h"
#include "HUD/VikingOverlay.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameModes/VikingGameState.h"

#include "DrawDebugHelpers.h"

AVikingCharacter::AVikingCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;


	//아래의 기능은 BP에서도 설정 할 수 있고 기능의 역할은
	//Controller를 회전시켰을 때 자연스럽게 캐릭터가 Controller의 앞쪽 방향으로 바라보게 하는 것이다.
	//바라보게하는 회전 속도 등 다양한 값을 바꿀 수 있다. 
	//-> GetCharacterMovement()->RotationRate = FRotator (0.f, 400.f, 0.f); Yaw방향으로 회전하는 속도를 400.f로 맞추는 것이다.
	//Only Idle만 true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	//Target Lock Init
	isTargetLocked = false;
	maxTargetingDis = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	//Grappling_Hook = CreateDefaultSubobject<UGrappling_Hook>(TEXT("Grappling_Hook"));
	//Grappling_Hook->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	AOEHitAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AOE Hit Audio Component"));
    AOEHitAudioComp->SetupAttachment(RootComponent);
	AOEHitAudioComp->bAutoActivate = false;

	VikingWeapon = CreateDefaultSubobject<UVikingWeapon>(TEXT("Viking Weapon"));

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
		EnhancedInputComponent->BindAction(VikingBowAim, ETriggerEvent::Triggered, this, &AVikingCharacter::BowAim);
		EnhancedInputComponent->BindAction(VikingReleaseBowAim, ETriggerEvent::Triggered, this, &AVikingCharacter::ReleaseBowAim);
		EnhancedInputComponent->BindAction(VikingBowShot, ETriggerEvent::Triggered, this, &AVikingCharacter::BowShot);
		EnhancedInputComponent->BindAction(VikingFirstSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::FirstSkill);
		EnhancedInputComponent->BindAction(VikingSecondSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::SecondSkill);
		EnhancedInputComponent->BindAction(VikingThirdSkill, ETriggerEvent::Triggered, this, &AVikingCharacter::ThirdSkill);
		EnhancedInputComponent->BindAction(VikingTargetLock, ETriggerEvent::Triggered, this, &AVikingCharacter::TargetLock_Release);
		EnhancedInputComponent->BindAction(VikingTargetChange, ETriggerEvent::Triggered, this, &AVikingCharacter::TargetChange);
	}
} 

void AVikingCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
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

	//Equip Init
	ChangeVikingEquip();
	CharacterState = ECharacterState::ESC_EquippingAxeAndShield;
	BowWidget->SetVisibility(ESlateVisibility::Collapsed);

	AGameStateBase* GameStateBase = GetWorld()->GetGameState();
	VikingGameState = Cast<AVikingGameState>(GameStateBase);

	//DodgeCurve Bind
	if (DodgeCameraCurve)
    {
        // Timeline 업데이트 함수를 Curve와 바인딩
        FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("DodgeCameraTimelineUpdate"));

        DodgeCameraTimeline.AddInterpFloat(DodgeCameraCurve, TimelineProgress);
        DodgeCameraTimeline.SetLooping(false); // 반복 여부 설정
    }
}

void AVikingCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if(isInvincible) return;		//회피중

	if(IsGuarding() && CanGuard(Hitter->GetActorLocation()) && Attributes){
		//
		if(CanParry && ParryMontage){
			ChoosePlayMontageSection(ParryMontage, TEXT("Parry"));
		}else{
			//UE_LOG(LogTemp, Display, TEXT("Guard Complete"));
			PlayGuardMontage();		
			Attributes->Heal(7.f);  //Guard시 TakeDamage를 if문으로 돌릴 방법을 찾지 못해서 일단 Heal을 하는 방식으로 적용			
		}

		//Shield->SpawnWeaponParticle();
		//Shield->PlayShieldSound(ImpactPoint);
	}else{
		//Guard 실패 || Guard중이 아닐 때
		Super::GetHit_Implementation(ImpactPoint, Hitter);
		
		PlayHitSound(ImpactPoint);
	}
	//In Viking GetHit_Implementation
	if(isAiming){
		//AOEHitAudio가 Hit당하는 소리라서 여기서도 재활용해서 Play
		AOEHitAudioComp->Play();
		isGetHitAiming = true;
		GetWorld()->GetTimerManager().SetTimer(AimingHitTimerHandle, this, &AVikingCharacter::GetHit_AimingRelease, 0.5f, false);
		ReleaseBowAim();
	}
 
	//공격 중간에 맞았을 때를 위한 상태 변화
	if(!IsGuarding() && Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
		ComboAttackIndex = 0;		//공격중에 클릭하고 나서 맞았을 경우에 ComboAttackIndex를 초기화해준다.
	}
	// SetWeaponCollision(Weapon, ECollisionEnabled::NoCollision);
	// SetWeaponCollision(Shield, ECollisionEnabled::NoCollision);
}

void AVikingCharacter::GetHitAOEAttack()
{
	if(AOEHitAudioComp){
		AOEHitAudioComp->Play();
	}
}

float AVikingCharacter::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	if(isInvincible) return 0;

    HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
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
				VikingOverlay->SetBowIndicatorVisible(false);
			}
		}
    }
}

void AVikingCharacter::ShowBossHealthBar(UHealthBar* BossHealthBar)
{
	if(BossHealthBar && VikingOverlay){
		VikingOverlay->AddBossHealthBarAtVertticalBox(BossHealthBar);
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

FRotator AVikingCharacter::GetAimRotation()
{
	FRotator AimRotaion;
	if(GetController()){
		AimRotaion = GetController()->GetControlRotation();
	}else{
		UE_LOG(LogTemp, Display, TEXT("GetController is null"));
	}
	return AimRotaion;
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
			StopVikingAutoAttackMontage();
		}
	}
}

void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DodgeCameraTimeline.TickTimeline(DeltaTime);

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

	if(ActionState == EActionState::EAS_Dodge){
		FVector CurrentLocation = GetActorLocation();
        FVector NewLocation = FMath::VInterpTo(CurrentLocation, DodgeTargetLocation, DeltaTime, DodgeSpeed);

        SetActorLocation(NewLocation);
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

	AddMovementInput(ForwardDirection, MoveValue.Y);
	AddMovementInput(RightDirection, MoveValue.X);
}

void AVikingCharacter::Look(const FInputActionValue &value)
{
	//카메라가 회전하는 작업.
	if(isTargetLocked) return;

	const FVector2D LookValue = value.Get<FVector2D>();

	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AVikingCharacter::Jump()
{
	//일시적 삭제
	if((IsUnoccupied() || IsGuarding()) && IsAlive())
	{
		Super::Jump();
	}
}

void AVikingCharacter::Equip()
{
	//UE_LOG(LogTemp, Display, TEXT("Equip Start "));
	if(isTargetLocked) TargetLock_Release();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && EquipMontage){
		AnimInstance->Montage_Play(EquipMontage);
		PlayAnimMontage(EquipMontage, 1, FName("Equip"));
		//ChangeCharacterState();
	}
}

void AVikingCharacter::Attack()
{	
	if(CharacterState != ECharacterState::ESC_EquippingAxeAndShield) return;
	if(IsAttacking())
	{
		ComboAttackIndex = 1;
	}

	if(CanAttack()){
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if(AnimInstance){
			PlayVikingAutoAttackMontage();
		}

		AttackRotate();
		
		ActionState = EActionState::EAS_Attacking;
		//AttackMotionWarpAnimNotify();
	}
}

void AVikingCharacter::PlayVikingAutoAttackMontage()
{
	//int32
	//return PlayRandomMontageSection(AutoAttackMontage, AutoAttackMontageSection);
	ChoosePlayMontageSection(AutoAttackMontage, TEXT("Default"));
}

void AVikingCharacter::StopVikingAutoAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance){
		AnimInstance->Montage_Stop(0.45f, AutoAttackMontage);
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
	if(!isDodgeCoolTimeEnd) return;
	if(!(IsUnoccupied() || IsGuarding() || IsSkilling()) ||
	!HasEnoughDodgeStamina() && VikingOverlay) return;

    ActionState = EActionState::EAS_Dodge;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    FVector DodgeDirection = CalculateDodgeDirection();
    DodgeTargetLocation = GetActorLocation() + (DodgeDirection * DodgeDistance);
	StartDodgeInvincibilityWindow();
	
	if(GetIsInEnemyAttackArea()){
		//UE_LOG(LogTemp, Warning, TEXT("Perfect Dodge"));
		//DodgeCameraTimeline.SetNewTime(0.0f);
		//DodgeCameraTimeline.Play();
		DodgeCameraTimeline.PlayFromStart();
		GetWorldSettings()->SetTimeDilation(0.3f);
		GetWorldTimerManager().SetTimer(DodgeTimeDilationTimerHandle, this, &AVikingCharacter::ResetGlobalTimeDilation, PerfectDodgeTimeDilationTime, false);
	}
	//연속해서 사용하지 못하게 함.
	isDodgeCoolTimeEnd = false;
}

void AVikingCharacter::StartDodgeInvincibilityWindow()
{
	isInvincible = true;
	GetWorldTimerManager().SetTimer(DodgeInvincibleTimerHandle, this, &AVikingCharacter::ResetInvincibility, InvincibilityTime, false);
}
void AVikingCharacter::ResetGlobalTimeDilation()
{
	GetWorldSettings()->SetTimeDilation(1.0f);
}

void AVikingCharacter::ResetInvincibility()
{
	isInvincible = false;
}

void AVikingCharacter::ResetDodgeState()
{
	//Animation Notify로 Animation이 끝날 때 쯤 함수 실행됌.
    isDodgeCoolTimeEnd = true;
}

void AVikingCharacter::DodgeCameraTimelineUpdate(float Value)
{
    // 카메라의 FOV를 Lerp로 변경
    float NewFOV = FMath::Lerp(InitialFieldOfView, DodgeFieldOfView, Value);
    if (Camera)
    {
        Camera->SetFieldOfView(NewFOV);
    }
}

bool AVikingCharacter::GetIsInEnemyAttackArea()
{
	return isInEnemyAttackArea;
}

void AVikingCharacter::SetIsInEnemyAttackArea(bool isIn)
{
	isInEnemyAttackArea = isIn;
}

FVector AVikingCharacter::CalculateDodgeDirection()
{
    // 현재 캐릭터 속도 가져오기
    FVector Velocity = GetVelocity();

    // 캐릭터의 회전 값 가져오기
    FRotator ActorRotation = GetActorRotation();

    // 캐릭터의 앞 방향 및 오른쪽 방향 계산
    FVector ForwardVector = ActorRotation.Vector(); // 캐릭터의 앞 방향 (X축)
    FVector RightVector = FRotationMatrix(ActorRotation).GetUnitAxis(EAxis::Y); // 캐릭터의 오른쪽 방향 (Y축)

    // 속도와 방향 성분 계산
    float ForwardValue = FVector::DotProduct(ForwardVector, Velocity); // 앞쪽 방향 성분
    float RightValue = FVector::DotProduct(RightVector, Velocity);     // 오른쪽 방향 성분

    // 최종 방향 벡터 계산
    FVector DodgeDirection = (ForwardVector * ForwardValue) + (RightVector * RightValue);

    // 벡터 정규화 (크기를 1로 유지)
    if (!DodgeDirection.IsNearlyZero())
    {
        DodgeDirection.Normalize();
    }
    else
    {
        // 속도가 없을 경우 기본 방향 설정 (앞 방향)
        DodgeDirection = ForwardVector;
    }

    return DodgeDirection;
}

void AVikingCharacter::Guard()
{
	if(!(IsUnoccupied() || IsGuarding()) || 
	!HasEnoughDodgeStamina() && VikingOverlay ||
	CharacterState != ECharacterState::ESC_EquippingAxeAndShield) return;

	ActionState = EActionState::EAS_Guard;
	GetCharacterMovement()->MaxWalkSpeed = GuardWalkSpeed;
	Attributes->UseStamina(Attributes->GetGuardCost());	

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

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	ActionState = EActionState::EAS_Unoccupied;

	CanParry = false;
}

bool AVikingCharacter::ParryCheck()
{
	return CanParry;
}

void AVikingCharacter::RestoreParryTimeDilation()
{
	//CanParry = true;  //필요한가? 일단 작성해보자
	this->SetCustiomTimeDilation(1.0f);
}

void AVikingCharacter::SetIsParryDilation(bool ParryDilation)
{
	isParryDilation = ParryDilation;
}

bool AVikingCharacter::GetIsParryDilation(){
	return isParryDilation;
}

void AVikingCharacter::MakeCantParry()
{
	CanParry = false;
}

//삭제 예정
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
	if(CharacterState != ECharacterState::ESC_EquippingAxeAndShield) return;

	if(isTargetLocked){
		//release 구현
		isTargetLocked = false;
		CombatTarget = nullptr;

		TargetLockOnEffects();
	}else{
		//TargetLock 구현
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
				CombatTarget = LockOnCandidates[0];
				TargetLockOnEffects();
				break;		//break가 없으면 for문을 돌아서 결국 0으로 온다.
			}else{
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
		if(Distance < 120.f){
			Distance = 120.f;
		}else if(Distance > 180.f){
			Distance = 180.f;
		}
	}

	return Distance;
}

void AVikingCharacter::SetVikingWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	if(VikingWeapon){
    	VikingWeapon->SetWeaponCollision(CollisionWeapon, CollisionType);
	}
}

AWeapon* AVikingCharacter::GetVikingWeapon()
{
	if(VikingWeapon->GetAxe()){
		return VikingWeapon->GetAxe();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Get Axe is Null (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
} 

AWeapon* AVikingCharacter::GetVikingShield()
{
	if(VikingWeapon->GetShield()){
		return VikingWeapon->GetShield();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("GetShield is Null (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}


ABow* AVikingCharacter::GetBow() const 
{
	if(VikingWeapon->GetBow()){
		return VikingWeapon->GetBow();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Get Bow is Null (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}

void AVikingCharacter::BowAim()
{
	//Input Action
	if(CharacterState != ECharacterState::ESC_EquippingBow) return;
	if(!isAiming && !isGetHitAiming){
		VikingOverlay->SetBowIndicatorVisible(true);
		ChoosePlayMontageSection(BowDrawingMontage, "BowDrawStart");
		ActionState = EActionState::EAS_Aiming;
		if(VikingWeapon->GetBow()){
			VikingWeapon->GetBow()->StartAiming();
		}
		isAiming = true;
	}
}

void AVikingCharacter::BowShot()
{
	//Input Action
	if(BowShotMontage && VikingWeapon && VikingWeapon->GetBow() && isAiming){
        ChoosePlayMontageSection(BowShotMontage, "BowShot");
		VikingWeapon->GetBow()->VikingBowShot();
	}
}

void AVikingCharacter::BowShotEnd()
{
	isAiming = false;
}

void AVikingCharacter::ReleaseBowAim()
{
	//InputAction
	if(ActionState != EActionState::EAS_Aiming) return;

	VikingOverlay->SetBowIndicatorVisible(false);
	ActionState = EActionState::EAS_Unoccupied;
	isAiming = false;

	if(VikingWeapon->GetBow()){
		VikingWeapon->GetBow()->StopAiming();
	}
}

void AVikingCharacter::GetHit_AimingRelease()
{
	isGetHitAiming = false;
}

void AVikingCharacter::AttackEnd()
{
	//UE_LOG(LogTemp, Display, TEXT("AttackEnd"));
	if(VikingWeapon->GetAxe() && VikingWeapon->GetShield()){
		VikingWeapon->GetAxe()->OverlappedActorClear();
		VikingWeapon->GetShield()->OverlappedActorClear();
	}
	//->OverlappedActorClear();
	//Shield->OverlappedActorClear();
}

bool AVikingCharacter::CanAttack()
{
	return IsUnoccupied() && !IsSkilling();
}

void AVikingCharacter::ChangeVikingEquip()
{
	ChangeCharacterState();
	if(VikingWeapon){
		VikingWeapon->ChangeEquip();
	}
}

void AVikingCharacter::ChangeCharacterState()
{
	if(CharacterState == ECharacterState::ESC_EquippingAxeAndShield)
	{	
		CharacterState = ECharacterState::ESC_EquippingBow;
	}else if (CharacterState == ECharacterState::ESC_EquippingBow)
	{
		CharacterState = ECharacterState::ESC_EquippingAxeAndShield;
	}
}

void AVikingCharacter::FinishEquipping()
{
	//이 부분도 나중에 AM이랑 Bind해서 끝나거나 Interrupt되면 실행되게 바꾸자이.
	ActionState = EActionState::EAS_Unoccupied;
}

void AVikingCharacter::PlayRollMontage(){
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && RollMontage){
		AnimInstance->Montage_Play(RollMontage);
		AnimInstance->Montage_JumpToSection(FName("Default"), RollMontage);
	}
}

void AVikingCharacter::EndHitReaction() 
{
	//이 부분도 나중에 AM이랑 Bind해서 끝나거나 Interrupt되면 실행되게 바꾸자이.
	ActionState = EActionState::EAS_Unoccupied;
}

void AVikingCharacter::EndDodge()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	GetWorldTimerManager().SetTimer(DodgeCooldownTimerHandle, this, &AVikingCharacter::ResetDodgeState, 0.5f, false);
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