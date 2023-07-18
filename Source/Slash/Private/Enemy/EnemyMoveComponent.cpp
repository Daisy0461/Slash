#include "Enemy/EnemyMoveComponent.h"
#include "AIController.h"

// Sets default values for this component's properties
UEnemyMoveComponent::UEnemyMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ParentActor = Cast<APawn>(GetOwner());
}

// Called when the game starts
void UEnemyMoveComponent::BeginPlay()
{
	Super::BeginPlay();

	//EnemyController = Cast<AAIController>(GetController());
	EnemyController = Cast<AAIController>(ParentActor->GetController());
	MoveToTarget(PatrolTarget);
}

// Called every frame
void UEnemyMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckPatrolTarget();
}

void UEnemyMoveComponent::MoveToTarget(AActor *Target)
{
	if(EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);		//Taget의 15.f 앞에 오면 도착한것으로 수락한다.
	EnemyController->MoveTo(MoveRequest);
}

void UEnemyMoveComponent::CheckPatrolTarget()
{
	if(InTargetRange(PatrolTarget, PatrolRadius))		//범위안에 있다면
	{
		const float WaitSec = FMath::RandRange(3.f, 8.f);
		PatrolTarget = ChoosePatrolTarget();			//Target을 설정하고
		ParentActor->GetWorldTimerManager().SetTimer(PatrolTimer, this, &UEnemyMoveComponent::PatrolTimerFinished, WaitSec);						//해당 Target으로 움직인다.
	}
}

bool UEnemyMoveComponent::InTargetRange(AActor *Target, double Radius)
{
	if(Target == nullptr) return false;

	//Target과의 거리 = DistanceToTarget
	const double DistanceToTarget = (Target->GetActorLocation() - ParentActor->GetActorLocation()).Size();
	//Target과의 거리가 Radiuse보다 작다면 true -> 적을 쫒아감
    return DistanceToTarget <= Radius;	
}

AActor *UEnemyMoveComponent::ChoosePatrolTarget()		//다음 PartrolTarget을 정한다.
{
   	TArray<AActor*> VaildTargets;
	for(AActor* target : PatrolTargets){
		if(target != PatrolTarget){
			VaildTargets.AddUnique(target);		//Vaild에는 현재 PatrolTarget이 아닌 것만 들어간다.
		}
	}

	if(VaildTargets.Num() > 0){
		const int rand = FMath::RandRange(0, VaildTargets.Num()-1);
		return VaildTargets[rand];
	}

	return nullptr;
}

void UEnemyMoveComponent::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}
