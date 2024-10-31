#include "Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Item/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	HitMoveValue = 100.f;
	AttackingMoveSpeed = 5.0f;
	HitMoveSpeed = 5.0f;
	bIsHitMoving = false;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Tick(float DeltaTime){
	Super::Tick(DeltaTime);

	if (bIsHitMoving)
    {
        const FVector CurrentLocation = GetActorLocation();
        const FVector NewLocation = FMath::Lerp(CurrentLocation, HitMoveLocation, DeltaTime * HitMoveSpeed);

        FHitResult HitResult;
        SetActorLocation(NewLocation, true, &HitResult);

        if (FVector::Dist(CurrentLocation, HitMoveLocation) < 50.f)		
        {
            bIsHitMoving = false;            
        }
    }

	//Attacking Move Lerp로 자연스럽게 가기
	//이건 나중에 Viking이랑 Enemy 모두 성공하고 해야지 이런식으로 하면 곤란해요
	if (bIsAttackingMove)
    {
        const FVector CurrentLocation = GetActorLocation();
        const FVector NewLocation = FMath::Lerp(CurrentLocation, TargetLocation, DeltaTime * AttackingMoveSpeed);

        FHitResult HitResult;
        SetActorLocation(NewLocation, true, &HitResult);

		//UE_LOG(LogTemp, Display, TEXT("DeltaTime: %f, AttackingMoveSpeed: %f"), DeltaTime, AttackingMoveSpeed);

		//값이 120.f보다 작아지면 Animation이 끝나기 전에 움직일 수가 있는데 이때 bIsAttackingMove가 true라서 움직일 수가 없다.
        if (FVector::Dist(CurrentLocation, TargetLocation) < 120.f)		
        {
            bIsAttackingMove = false;
            //UE_LOG(LogTemp, Display, TEXT("Attacking Move completed"));
        }
    }
}

void ABaseCharacter::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	if(IsAlive()){
		DirectionalHitReact(Hitter->GetActorLocation());
	}else{
		//UE_LOG(LogTemp, Display, TEXT("Hit Die"));
		Die();
	}
}

void ABaseCharacter::SetWeaponCollision(AWeapon* CollisionWeapon,ECollisionEnabled::Type CollisionType)
{
	if(CollisionWeapon && CollisionWeapon->GetWeaponBox())
	{	
		CollisionWeapon->IgnoreActors.Empty();
		CollisionWeapon->IgnoreActors.Add(GetOwner());

		CollisionWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionType);
	}else if(!CollisionWeapon){
		UE_LOG(LogTemp, Display, TEXT("Can't Find Weapon"));
	}
}

void ABaseCharacter::SetEquippedWeapon(AWeapon* InputWeapon){
	Weapon = InputWeapon;
}

AWeapon* ABaseCharacter::GetWeapon(){
	return Weapon;
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


void ABaseCharacter::StopAutoAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance){
		AnimInstance->Montage_Stop(0.45f, AutoAttackMontage);
	}
}

void ABaseCharacter::Attack()
{
	if(CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
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

void ABaseCharacter::PlayHitReactMontage(const FName &SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage){
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
		//UE_LOG(LogTemp, Display, TEXT("SectionName :%s"), *SectionName.ToString());
	}
}

void ABaseCharacter::HitMove(FName moveDirection)
{

}

float ABaseCharacter::GetHitMoveValue()
{
	return HitMoveValue;
}

void ABaseCharacter::SetHitMoveValue(float value)
{
	HitMoveValue = value;
}

bool ABaseCharacter::CanAttack()
{
    return false;
}

void ABaseCharacter::SetHitting()
{
}

void ABaseCharacter::GetHittingEnd()
{
}

float ABaseCharacter::CheckTargetDistance()
{
	float Distance = AttackMoveMaxDistance;
	if(CombatTarget){
		Distance = GetDistanceTo(CombatTarget);

		if(Distance < 120.f){
			Distance = 120.f;
		}else if(Distance > AttackMoveMaxDistance){
			Distance = AttackMoveMaxDistance;
		}
	}

	//UE_LOG(LogTemp, Display, TEXT("Distance : %f"), Distance);

	return Distance;
}

void ABaseCharacter::AttackingMoveLocating()
{
	if (GetCharacterMovement())
    {
        FVector Direction = GetActorForwardVector();
		float moveValue = CheckTargetDistance();
        TargetLocation = GetActorLocation() + (Direction * (moveValue-30));
        bIsAttackingMove = true; // 이동 시작 플래그 설정
        //UE_LOG(LogTemp, Display, TEXT("Attacking Move started"));
    } 
    else
    {
        //UE_LOG(LogTemp, Display, TEXT("GetCharacterMovement failed"));
    }
}

void ABaseCharacter::AttackRotate()
{
	//위와 동일하게 파마리터 들어오면 가능할듯? 일단 만들어지는지 확인해보고.
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

	FName HitSectionName = FName("FromBack");
	if(Theta < 45.f && -45.f <=Theta){
		HitSectionName = FName("FromFront");
	}else if(Theta < -45.f && -135.f <= Theta){
		HitSectionName = FName("FromLeft");
	}else if(Theta < 135.f && 45.f <= Theta){
		HitSectionName = FName("FromRight");
	}

	PlayHitReactMontage(HitSectionName);

	FVector HitDirection = ToHit * -1.f; 	// 맞은 방향의 반대 방향으로 이동
    float HitDistance = 100.f; 				// 이동 거리값.
    HitMoveLocation = GetActorLocation() + (HitDirection * HitDistance);
    bIsHitMoving = true;
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
	// UE_LOG(LogTemp, Display, TEXT("Is Alive %s   // Actor : %s"), Attributes->IsAlive() ? TEXT("true") : TEXT("false"), 
	// 	*GetName());
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
		
	}

	return Selection;
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// FString ABaseCharacter::GetEnumDisplayNameToString(const TCHAR * Enum, int32 EnumValue) const
// {
// 	//UE_LOG(LogTemp, Display, TEXT("EDeathPose: %s"), *GetEnumDisplayNameToString(TEXT("EDeathPose"), static_cast<uint8>(Selection)));
// 	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, Enum, true);
// 	if (EnumPtr == nullptr)
// 	{
// 		return FString("Invalid");
// 	}

// 	return EnumPtr->GetDisplayNameTextByIndex(EnumValue).ToString();
// }