// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyThrowWeaponAttackComponent.h"
//#include "Item/Weapons/ThrowWeapon.h"
#include "Enemy/Enemy.h"

// Sets default values for this component's properties
UEnemyThrowWeaponAttackComponent::UEnemyThrowWeaponAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	OwnerEnemy = Cast<AEnemy>(GetOwner());
	ThrowPosition = CreateDefaultSubobject<USceneComponent>(TEXT("ThrowPosition"));
}


// Called when the game starts
void UEnemyThrowWeaponAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!OwnerEnemy){
		AActor* Owner = GetOwner();
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr : %s (%s)"), *Owner->GetName(), *FPaths::GetCleanFilename(__FILE__));
	}else{
		ThrowPosition->AttachToComponent(OwnerEnemy->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UEnemyThrowWeaponAttackComponent::PlayThrowWeaponAttackMontage()
{
	if(ThrowWeaponAttackMontageSection.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ThrowWeaponAttackMontageSection is Empty (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!ThrowWeaponAttackMontage || !OwnerEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("ThrowWeaponAttackMontage or OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 SectionIndex = FMath::RandRange(0, ThrowWeaponAttackMontageSection.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(ThrowWeaponAttackMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(ThrowWeaponAttackMontageSection[SectionIndex], ThrowWeaponAttackMontage);
}

void UEnemyThrowWeaponAttackComponent::StopThrowWeaponAttackMontage()
{
	if(!OwnerEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	UAnimInstance* AnimInstance = OwnerEnemy->GetEnemyAnimInstance();
	if(AnimInstance && ThrowWeaponAttackMontage)
	{
		AnimInstance->Montage_Stop(0.45f, ThrowWeaponAttackMontage);
	}
}

void UEnemyThrowWeaponAttackComponent::SpawnThrowWeapon()
{
	if(ThrowWeapon && OwnerEnemy){
		const FVector ThrowPositionLocation = ThrowPosition->GetComponentLocation();
		FRotator SpawnRotation = OwnerEnemy->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerEnemy; 
		SpawnParams.Instigator = OwnerEnemy->GetInstigator();

		AActor* SpawnedWeapon = OwnerEnemy->GetWorld()->SpawnActor<AActor>(ThrowWeapon, ThrowPositionLocation, OwnerEnemy->GetActorRotation());
		if(SpawnedWeapon){
			SpawnedWeapon->SetOwner(OwnerEnemy);
			SpawnedWeapon->SetInstigator(Cast<APawn>(OwnerEnemy));
		}else{
			UE_LOG(LogTemp, Warning, TEXT("Throw Weapon Spawn Fail (%s)"), *FPaths::GetCleanFilename(__FILE__));
		}
	}else{
		UE_LOG(LogTemp, Warning, TEXT("ThrowWeapon is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}