#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Item/Weapons/Shield.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	if(IsAlive()){
		DirectionalHitReact(Hitter->GetActorLocation());
	}else{
		Die();
	}
}

void ABaseCharacter::AttackMotionWarp_Implementation()
{
	
}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionType)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{	
		//UE_LOG(LogTemp, Display, TEXT("Your message"));
		EquippedWeapon->IgnoreActors.Empty();
		EquippedWeapon->IgnoreActors.Add(GetOwner());

		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
	}else if(!EquippedWeapon){
		//UE_LOG(LogTemp, Display, TEXT("Can't Find Equipped Weapon"));
	}
}

void ABaseCharacter::AttackMotionWarpAnimNotify()
{
	Execute_AttackMotionWarp(this);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage *Montage, const TArray<FName> &SectionName)
{
	if(SectionName.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionName.Num()-1;
	const int32 Selction = FMath::RandRange(0, MaxSectionIndex);
	ChoosePlayMontageSection(Montage, SectionName[Selction]);
    return Selction;
}

void ABaseCharacter::ChoosePlayMontageSection(UAnimMontage* Montage, const FName &SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && Montage){
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayAutoAttackMontage()
{
	return PlayRandomMontageSection(AutoAttackMontage, AutoAttackMontageSection);
}

int32 ABaseCharacter::PlayMotionWarpAttackMontage()
{
	return PlayRandomMontageSection(MotionWarpAttackMontage, MotionWarpAttackMontageSection);
}

void ABaseCharacter::StopAutoAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance){
		AnimInstance->Montage_Stop(0.45f, AutoAttackMontage);
	}
}

void ABaseCharacter::StopMotionWarpAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance){
		AnimInstance->Montage_Stop(0.45f, MotionWarpAttackMontage);
	}
}

void ABaseCharacter::Attack()
{
	if(CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName &SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage){
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
		UE_LOG(LogTemp, Display, TEXT("SectionName :%s"), *SectionName.ToString());
	}
}

bool ABaseCharacter::CanAttack()
{
    return false;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::SetHitting()
{
}

void ABaseCharacter::GetHittingEnd()
{
}


FVector ABaseCharacter::GetTransltaionWarpTarget()
{
	if(CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe = TargetToMe*WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}
FVector ABaseCharacter::GetRotationWarpTarget()
{
	if(CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
    return FVector();
}

void ABaseCharacter::PlayHitSound(const FVector &ImpactPoint)
{
	if(HitSound){
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticle(const FVector &ImpactPoint)
{
	if(HitParticles && GetWorld()){
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector &ImpactPoint)
{
	const FVector EnemyForward = GetActorForwardVector();
	//무기가 휘두르는대로 Vector를 구하게되면 Z축의 위치 때문에 Vector가 위로 뻗거나 아래로 뻗는 것을 방지하기 위해 아래 코드가 들어간다.
	const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	//Forward * ToHit = |Forward| * |ToHit| * cos(theta) = cos(theta)
	const double CosTheta = FVector::DotProduct(EnemyForward, ToHit);
	//ACos(Cos(Theta)) = Theta이고 Radian의 값을 가진다.
	double Theta = FMath::Acos(CosTheta);
	//Radian을 Degree의 값을 바꾼다.
	Theta = FMath::RadiansToDegrees(Theta);

	/*
	CrossProduct는 두 Vector와 직교하는 Vector를 구한다. 이때 Vector와 직교하는 Vector는 두개가 나오게 된다.
	//Unreal에서는 왼손을 사용하면 쉽게 알 수 있는데 왼손 검지가 첫번째(EnemyForward), 중지가 두번째(ToHit) 파라미터가 된다.
	//이때 검지와 중지를 알맞게 하고 엄지가 위로가는지 밑으로 가는지를 통해 +, -를 알 수 있다. 결국 엄지의 방향이 CrossProduct의 방향이다.
	//엄지가 아래로 가면 -, 위로가면 +이다.
	CrossProduct 사용하는 이유는 두 Vector의 값이 항상 0<=Theta<+180이기 때문에 각도로 왼쪽인지 오른쪽인지 구분을 할 수 없기 때문이다.
	*/
	const FVector CrossProduct = FVector::CrossProduct(EnemyForward, ToHit);
	if(CrossProduct.Z < 0){		//-일떄만 Theta값 -값으로 바꿔주면 된다.
		Theta = -1.f * Theta;
	}

	FName SectionName("FromBack");
	if(Theta < 45.f && -45.f <=Theta){
		SectionName = FName("FromFront");
	}else if(Theta < -45.f && -135.f <= Theta){
		SectionName = FName("FromLeft");
	}else if(Theta < 135.f && 45.f <= Theta){
		SectionName = FName("FromRight");
	}

	PlayHitReactMontage(SectionName);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if(Attributes){
		Attributes->ReceiveDamage(DamageAmount);	
	}
}

void ABaseCharacter::Die() 
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
}

bool ABaseCharacter::IsAlive()
{
    return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableCapsuleCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSection);
	//UE_LOG(LogTemp, Warning, TEXT("Die Selection: %d"), Selection);		
	TEnumAsByte<EDeathPose> Pose(Selection);

	if(Pose < EDeathPose::EDP_Max){
		DeathPose = Pose;
		//UE_LOG(LogTemp, Display, TEXT("EDeathPose: %s"), *GetEnumDisplayNameToString(TEXT("EDeathPose"), static_cast<uint8>(Selection)));
	}

	return Selection;
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
// FString ABaseCharacter::GetEnumDisplayNameToString(const TCHAR * Enum, int32 EnumValue) const
// {
// 	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
// 	if (EnumPtr == nullptr)
// 	{
// 		return FString("Invalid");
// 	}

// 	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
// }