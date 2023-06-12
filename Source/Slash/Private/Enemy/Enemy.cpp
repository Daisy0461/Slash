#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attribute"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//Play_Warrior_HitReact_Montage(FName("FromRight"));

	if(HitSound){		//Sound 재생
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if(HitParticles && GetWorld()){		//Particle 재생
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles, 
			ImpactPoint
		);
	}

	if(Attributes && Attributes->IsAlive()){		//살아있기 때문에 Hit React 재생
		//UE_LOG(LogTemp, Display, TEXT("Your message"));
		DirectionalHitReact(ImpactPoint);
	}

	if(Attributes && !Attributes->IsAlive()){
		Die_Montage(SelectDieAnimation());
	}
	
}
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	if(Attributes && HealthBarWidget){
		Attributes->ReceiveDamage(DamageAmount);	
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
    return DamageAmount;
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint){
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

	Play_Warrior_HitReact_Montage(SectionName);
}


void AEnemy::Play_Warrior_HitReact_Montage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage){
		//UE_LOG(LogTemp, Display, TEXT("IN IF"));
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::Die_Montage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && DieMontage){
		AnimInstance->Montage_Play(DieMontage);
		AnimInstance->Montage_JumpToSection(SectionName, DieMontage);
	}
}

FName AEnemy::SelectDieAnimation()
{
	int32 SelectNum = FMath::RandRange(0, 4);
	FName SectionName = FName("Death_1");
	switch (SelectNum)
	{
	case 1:
		SectionName = FName("Death_1");
		DeathPose = EDeathPose::EDP_Dead_1;
		break;
	case 2:
		SectionName = FName("Death_2");
		DeathPose = EDeathPose::EDP_Dead_2;
		break;
	case 3:
		SectionName = FName("Death_3");
		DeathPose = EDeathPose::EDP_Dead_3;
		break;
	case 4:
		SectionName = FName("Death_4");
		DeathPose = EDeathPose::EDP_Dead_4;
		break;
	case 5:
		SectionName = FName("Death_5");
		DeathPose = EDeathPose::EDP_Dead_5;
		break;
	default:
		break;
	}

    return SectionName;
}
