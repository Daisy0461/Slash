#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
}

void ABaseCharacter::PlayHitReactMontage(const FName &SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage){
		//UE_LOG(LogTemp, Display, TEXT("IN IF"));
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
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
		//UE_LOG(LogTemp, Display, TEXT("Front"));
	}else if(Theta < -45.f && -135.f <= Theta){
		SectionName = FName("FromLeft");
		//UE_LOG(LogTemp, Display, TEXT("Left"));
	}else if(Theta < 135.f && 45.f <= Theta){
		SectionName = FName("FromRight");
		//UE_LOG(LogTemp, Display, TEXT("Right")); 
	}

	PlayHitReactMontage(SectionName);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if(EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
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

bool ABaseCharacter::CanAttack()
{
    return false;
}
void ABaseCharacter::AttackEnd()
{
}
void ABaseCharacter::Die()
{
}
void ABaseCharacter::DisableCapsuleCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if(Attributes){
		Attributes->ReceiveDamage(DamageAmount);	
	}
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
		AnimInstance -> Montage_Play(Montage);
		
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSection);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomMontageSection(DeathMontage, DeathMontageSection);
}