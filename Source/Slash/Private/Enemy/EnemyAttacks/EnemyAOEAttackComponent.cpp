// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/EnemyAOEAttack.h"
#include "Enemy/EnemyFollowingAOEAttack.h"
#include "Enemy/EnemyAreaHeal.h"
#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Components/TimelineComponent.h"

// Sets default values for this component's properties
UEnemyAOEAttackComponent::UEnemyAOEAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;		//Spin때문에 true로 변경

	OwnerEnemy = Cast<AEnemy>(GetOwner());
}


// Called when the game starts
void UEnemyAOEAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if(!OwnerEnemy){
		AActor* Owner = GetOwner();
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr : %s (%s)"), *Owner->GetName(), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	//SpinningAttack
	//실제로 TimeLine이 있는지 확인해야함.
	if(SpinCurve){
		FOnTimelineFloat SpinMeshTimelineCall;
		SpinMeshTimelineCall.BindUFunction(this, FName("SpinMesh"));
		SpinMeshTimeline.AddInterpFloat(SpinCurve, SpinMeshTimelineCall);
		SpinMeshTimeline.SetLooping(false);
	}
}

void UEnemyAOEAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(SpinMeshTimeline.IsPlaying()){
        SpinMeshTimeline.TickTimeline(DeltaTime);
    }
}

void UEnemyAOEAttackComponent::EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType)
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	switch (AOEAttackType)
    {
    case EEnemyAOEAttackEnum::EEAA_SpinningAttack:
        PlaySpinningAttackMontage();
        break;
    case EEnemyAOEAttackEnum::EEAA_GroundAttack:
        PlayGroundAttackMontage();
        break;
	case EEnemyAOEAttackEnum::EEAA_FollowingAreaAttack:
		PlayFollowingAreaMontage();
		break;
    case EEnemyAOEAttackEnum::EEAA_MagicAreaAttack:
        PlayMagicAreaAttackMontage();
        break;
    case EEnemyAOEAttackEnum::EEAA_HealingArea:
        PlayHealingAreaMontage();
        break;
    default:
        UE_LOG(LogTemp, Warning, TEXT("Invalid Attack Type"));
        break;
    }
}

void UEnemyAOEAttackComponent::PlaySpinningAttackMontage()
{	
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!SpinningAttackMontage){
		UE_LOG(LogTemp, Warning, TEXT("SpinningAttackMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(SpinningAttackMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("SpinningAttackMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	
	int32 RandomIndex = FMath::RandRange(0, SpinningAttackMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(SpinningAttackMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(SpinningAttackMontageSections[RandomIndex], SpinningAttackMontage);
}

void UEnemyAOEAttackComponent::SpinMeshTimelineControll(bool bIsStart)
{
	//ABP에서 Spin시작할 때 사용 됌
	if(bIsStart){
		SpinMeshTimeline.PlayFromStart();
	}else{
		if(SpinMeshTimeline.IsPlaying()){
			SpinMeshTimeline.Stop();
		}
	}
}

void UEnemyAOEAttackComponent::SpinAOESpawn()
{
	if(SpinningAOEAttack){
		FVector SpawnLocation = OwnerEnemy->GetActorLocation();
		FRotator SpawnRotation = OwnerEnemy->GetActorRotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerEnemy; 
		SpawnParams.Instigator = OwnerEnemy->GetInstigator();

		GetWorld()->SpawnActor<AEnemyAOEAttack>(SpinningAOEAttack, SpawnLocation, SpawnRotation, SpawnParams);
	}else{
		UE_LOG(LogTemp, Warning, TEXT("SpinningAOEAttack is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UEnemyAOEAttackComponent::SpinMesh(float Value)
{
    FRotator NewRotation = FRotator(0.f, 360.f * Value * SpinValue, 0.f) + FRotator(0.f, -90.f, 0.f);
    OwnerEnemy->GetMesh()->SetRelativeRotation(NewRotation);
}

void UEnemyAOEAttackComponent::PlayGroundAttackMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!GroundAttackMontage){
		UE_LOG(LogTemp, Warning, TEXT("GroundAttackMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(GroundAttackMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("GroundAttackMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	
	int32 RandomIndex = FMath::RandRange(0, GroundAttackMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(GroundAttackMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(GroundAttackMontageSections[RandomIndex], GroundAttackMontage);
}

void UEnemyAOEAttackComponent::SpawnGroundAOE()
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!GroundAOEClass){
		UE_LOG(LogTemp, Warning, TEXT("GroundAOEClass is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	FVector GroundLocation = OwnerEnemy->GetGroundLocation(OwnerEnemy);
	FVector ForwardOffset = OwnerEnemy->GetActorForwardVector() * GroundForwardOffset;
	FVector SpawnLocation = GroundLocation + ForwardOffset;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerEnemy; 
	SpawnParams.Instigator = OwnerEnemy->GetInstigator();
	GetWorld()->SpawnActor<AEnemyAOEAttack>(GroundAOEClass, SpawnLocation, OwnerEnemy->GetActorRotation(), SpawnParams);
}

void UEnemyAOEAttackComponent::PlayFollowingAreaMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!FollowingAreaAttackMontage){
		UE_LOG(LogTemp, Warning, TEXT("FollowingAreaAttackMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(FollowingAreaAttackMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("FollowingAreaAttackMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	
	int32 RandomIndex = FMath::RandRange(0, FollowingAreaAttackMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(FollowingAreaAttackMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(FollowingAreaAttackMontageSections[RandomIndex], FollowingAreaAttackMontage);
}

void UEnemyAOEAttackComponent::SpawnFollowingAOE()
{
	if (!OwnerEnemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }
    if (!FollowingAreaAOEClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("FollowingAreaAOEClass is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
        return;
    }

    AActor* AttackTarget = OwnerEnemy->GetBaseEnemyAIController()->GetAttackTargetActor();
    if (AttackTarget)
    {
        FVector FollowingAreaLocation = OwnerEnemy->GetGroundLocation(AttackTarget);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = OwnerEnemy;
        SpawnParams.Instigator = OwnerEnemy->GetInstigator();
        FollowingAOE = GetWorld()->SpawnActor<AEnemyFollowingAOEAttack>(FollowingAreaAOEClass, FollowingAreaLocation, OwnerEnemy->GetActorRotation(), SpawnParams);

        UE_LOG(LogTemp, Display, TEXT("BlendingOut Following"));

        // Delegate 등록: BlendingOutStarted만 사용
        FOnMontageBlendingOutStarted BlendingOutDelegate = FOnMontageBlendingOutStarted::CreateUObject(this, &UEnemyAOEAttackComponent::OnFollowingAOEMontageBlendingOut);
        OwnerEnemy->GetEnemyAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, FollowingAreaAttackMontage);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FollowingAOE AttackTarget is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void UEnemyAOEAttackComponent::OnFollowingAOEMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("In Following AOE Blending Out"));
	if (Montage == FollowingAreaAttackMontage)
	{
		DestroyFollowingAOEArea();
	}
}

void UEnemyAOEAttackComponent::DestroyFollowingAOEArea()
{
	if(FollowingAOE){
		FollowingAOE->Destroy();
		FollowingAOE = nullptr;
	}
}

void UEnemyAOEAttackComponent::PlayMagicAreaAttackMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!MagicAreaAttackMontage){
		UE_LOG(LogTemp, Warning, TEXT("MagicAreaAttackMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(MagicAreaAttackMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("MagicAreaAttackMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, MagicAreaAttackMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(MagicAreaAttackMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(MagicAreaAttackMontageSections[RandomIndex], MagicAreaAttackMontage);
}

void UEnemyAOEAttackComponent::SpawnMagicAreaAOE()
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!MagicAreaAOEClass){
		UE_LOG(LogTemp, Warning, TEXT("MagicAreaAOEClass is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	AActor* AttackTarget = OwnerEnemy->GetBaseEnemyAIController()->GetAttackTargetActor();
	if(AttackTarget){
		FVector MagicAreaLocation = OwnerEnemy->GetGroundLocation(AttackTarget);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = OwnerEnemy; 
		SpawnParams.Instigator = OwnerEnemy->GetInstigator();
		GetWorld()->SpawnActor<AEnemyAOEAttack>(MagicAreaAOEClass, MagicAreaLocation, OwnerEnemy->GetActorRotation(), SpawnParams);
	}else{
		UE_LOG(LogTemp, Warning, TEXT("MagicArea AttackTarget is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void UEnemyAOEAttackComponent::PlayHealingAreaMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!HealingAreaMontage){
		UE_LOG(LogTemp, Warning, TEXT("HealingAreaMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(HealingAreaMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("HealingAreaMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, HealingAreaMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(HealingAreaMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(HealingAreaMontageSections[RandomIndex], HealingAreaMontage);
	SpawnHealingArea();
}

void UEnemyAOEAttackComponent::SpawnHealingArea()
{
	if(!HealingAreaClass){
		UE_LOG(LogTemp, Warning, TEXT("HealingAreaClass is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	HealingArea = GetWorld()->SpawnActor<AEnemyAreaHeal>(HealingAreaClass, OwnerEnemy->GetGroundLocation(OwnerEnemy), OwnerEnemy->GetActorRotation());

	FOnMontageBlendingOutStarted BlendingOutDelegate = FOnMontageBlendingOutStarted::CreateUObject(this, &UEnemyAOEAttackComponent::OnHealingMontageBlendingOut);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_SetBlendingOutDelegate(BlendingOutDelegate, HealingAreaMontage);

}

void UEnemyAOEAttackComponent::OnHealingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HealingAreaMontage)
	{
		DestroyHealingArea();
	}
}

void UEnemyAOEAttackComponent::DestroyHealingArea()
{
	if(HealingArea){
		HealingArea->Destroy();
		HealingArea = nullptr;
	}
}
