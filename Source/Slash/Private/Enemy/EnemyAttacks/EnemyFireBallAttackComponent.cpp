// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"

// Sets default values for this component's properties
UEnemyFireBallAttackComponent::UEnemyFireBallAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	OwnerEnemy = Cast<AEnemy>(GetOwner());

	FirePosition = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));

}


// Called when the game starts
void UEnemyFireBallAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerEnemy){
		FirePosition->AttachToComponent(OwnerEnemy->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UEnemyFireBallAttackComponent::EnemyFireBallAttack(EEnemyFireBallEnum FireBallType)
{
	if(!OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy is nullptr %s (%s)"), *OwnerEnemy->GetName(), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	switch (FireBallType)
	{
	case EEnemyFireBallEnum::EFBE_BasicFireBall:
		PlayBasicFireBallMontage();
		break;
	case EEnemyFireBallEnum::EFBE_BarrageFireBall:
		PlayFireBallBarrageMontage();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid FireBall Type"));
		break;
	}
}

void UEnemyFireBallAttackComponent::PlayBasicFireBallMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!BasicFireBallMontage){
		UE_LOG(LogTemp, Warning, TEXT("BasicFireBallMontage is nullptr(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!BasicFireBallMontageSections.Num()){
		UE_LOG(LogTemp, Warning, TEXT("BasicFireBallMontageSections is Empty(%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, BasicFireBallMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(BasicFireBallMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(BasicFireBallMontageSections[RandomIndex], BasicFireBallMontage);
}

void UEnemyFireBallAttackComponent::SpawnBasicFireBall()
{
	if(!FirePosition || !BasicFireBall || !OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("SpawnBasicFireBall Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	const FVector FirePositionLocation = FirePosition->GetComponentLocation();
	//const FTransform FirePositionTransform = FirePosition->GetComponentTransform();
	
	AActor* SpawnedFireBall = OwnerEnemy->GetWorld()->SpawnActor<AActor>(BasicFireBall, FirePositionLocation, OwnerEnemy->GetActorRotation());
	if(SpawnedFireBall){
		SpawnedFireBall->SetOwner(OwnerEnemy);
		SpawnedFireBall->SetInstigator(Cast<APawn>(OwnerEnemy));
    }else{
        UE_LOG(LogTemp, Warning, TEXT("FireBall Spawn Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
    }
}

void UEnemyFireBallAttackComponent::PlayFireBallBarrageMontage()
{
	if(!OwnerEnemy->GetEnemyAnimInstance()){
		UE_LOG(LogTemp, Warning, TEXT("OwnerEnemy AnimInstance is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(!BarrageFireBallMontage){
		UE_LOG(LogTemp, Warning, TEXT("BarrageFireBallMontage is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}
	if(BarrageFireBallMontageSections.Num() == 0){
		UE_LOG(LogTemp, Warning, TEXT("BarrageFireBallMontageSections is Empty (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	int32 RandomIndex = FMath::RandRange(0, BarrageFireBallMontageSections.Num() - 1);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_Play(BarrageFireBallMontage);
	OwnerEnemy->GetEnemyAnimInstance()->Montage_JumpToSection(BarrageFireBallMontageSections[RandomIndex], BarrageFireBallMontage);
}

void UEnemyFireBallAttackComponent::SpawnBarrageFireBall()
{
	if(!FirePosition || !BarrageFireBall || !OwnerEnemy){
		UE_LOG(LogTemp, Warning, TEXT("SpawnBarrageFireBall Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return;
	}

	AActor* AttackTarget = OwnerEnemy->GetBaseEnemyAIController()->GetAttackTargetActor();
	if(!AttackTarget){
		UE_LOG(LogTemp, Warning, TEXT("AttackTarget is nullptr (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}

	const FVector FirePositionLocation = FirePosition->GetComponentLocation();
	//const FTransform FirePositionTransform = FirePosition->GetComponentTransform();

	FVector RandomLocation = AttackTarget->GetActorLocation() + FVector(0.f, FMath::RandRange(-100.f, 100.f), 0.f);
	FVector Direction = (RandomLocation - OwnerEnemy->GetActorLocation()).GetSafeNormal();
	FRotator RandomRotation = Direction.Rotation();

	AActor* SpawnBarrageFireBall = OwnerEnemy->GetWorld()->SpawnActor<AActor>(BarrageFireBall, FirePositionLocation, RandomRotation);

	if(SpawnBarrageFireBall){
		SpawnBarrageFireBall->SetOwner(OwnerEnemy);
		SpawnBarrageFireBall->SetInstigator(Cast<APawn>(OwnerEnemy));
	}else{
		UE_LOG(LogTemp, Warning, TEXT("FireBall Spawn Failed (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}
