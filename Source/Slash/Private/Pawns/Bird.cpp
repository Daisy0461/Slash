// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetCapsuleHalfHeight(20.f);
	CapsuleComp->SetCapsuleRadius(15.f);
	SetRootComponent(CapsuleComp);	// == RootComponent = CapsuleComp;

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BridMesh"));
	BirdMesh->SetupAttachment(CapsuleComp);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());		//Controller를 PlayerController로 Cast해서 *가 존재하는지 확인한다.
	if(PlayerController)
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			//BridMappingContext는 BP에서 명시적으로 지정해주며 두번째는 우선순위를 나타낸다.
			Subsystem->AddMappingContext(BridMappingContext, 0);
		}
	}
	
}


// 여기서 FInputActionValue는 InputAction으로 받을 수 있는 bool, float, Vector2D, Vector3D 값을 가질 수 있다.
void ABird::Move(const FInputActionValue& value)
{	
	//Get함수는 FInputActionValue가 다양한 값들을 가질 수 있기 때문에 <>로 묶여진 값을 얻어온다.
	const FVector2D CurrentValue = value.Get<FVector2D>();
	if(Controller && (CurrentValue.Y != 0.f)){\
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, CurrentValue.Y);
	}
}

void ABird::Look(const FInputActionValue &value)
{
	const FVector2D LookAxisValue = value.Get<FVector2D>();
	if(Controller){
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//CastCheck는 Cast가 되지 않은 타입일 때 Cast를 시키지 않는다.
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//BindAxis와 비슷하게 MoveAction(변수)이라는 IA를 사용할 것이고 Triggerd될 때 사용할 것이며 이 스크립트가 적용된 Pawn에 수행을 하며 Brid::Move함수를 실행한다.
		EnhancedInputComponent -> BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
		EnhancedInputComponent -> BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
	}
}