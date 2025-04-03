#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//Attribute는 삭제 예정 - HealthWidget이 있음.
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "TimerManager.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "HUD/HealthBarComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Health.h"
#include "Item/Weapons/Weapon.h"
#include "UObject/Class.h"
#include "Perception/AISense_Damage.h"
#include "HUD/HealthBar.h"
#include "DrawDebugHelpers.h"
//Slice
#include "KismetProceduralMeshLibrary.h" 
#include "ProceduralMeshComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODModel.h"
#include "NavigationSystem.h"
#include "Materials/MaterialInstance.h" 


//Attack
#include "Enemy/EnemyAttacks/EnemyAutoAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyThrowWeaponAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyAOEAttackEnum.h"
#include "Enemy/EnemyAttacks/EnemyFireBallAttackComponent.h"
#include "Enemy/EnemyAttacks/EnemyFireBallEnum.h"
#include "Enemy/EnemyAttacks/EnemyTeleportComponent.h"
#include "Enemy/EnemyAttacks/EnemyTeleportEnum.h"

AEnemy::AEnemy() 
{ 
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Enemy"));

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	//Components 추가
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	HealthBarWidget->SetGenerateOverlapEvents(false);

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(GetRootComponent());

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	bUseControllerRotationPitch = false;
	//strafe를 위해 true를 해야함. 
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	CombatTarget = nullptr;

	PrimaryActorTick.bCanEverTick = true;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//HealthBarWiget 최초에 숨기기
	SetMovementSpeedEnum(EEnemyMovementSpeed::EEMS_Idle);
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(false);
	}

	if(HeadShotCurve){
		FOnTimelineFloat TimelineProgress;
        TimelineProgress.BindUFunction(this, FName("HeadShotReaction"));

        HeadShotTimeline.AddInterpFloat(HeadShotCurve, TimelineProgress);
        HeadShotTimeline.SetLooping(false); // 반복 여부 설정

		FOnTimelineEvent TimelineFinished;
		TimelineFinished.BindUFunction(this, FName("HeadShotReactionEnd"));
		HeadShotTimeline.SetTimelineFinishedFunc(TimelineFinished);
	}

	BaseEnemyAIController = Cast<ABaseEnemyAIController>(GetController());
	if(!BaseEnemyAIController){
		UE_LOG(LogTemp, Warning, TEXT("Base Enemy AI Controller is Null"));
	}
	// AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
    // {
	// 	TRACE_CPUPROFILER_EVENT_SCOPE(SelectVertices_Async);
    //     this->SelectVertices(0);
    // });

	//Test 반드시 삭제해야함.
	TRACE_CPUPROFILER_EVENT_SCOPE(CopySkeletalMeshToProcedural);
	CopySkeletalMeshToProcedural(0);
	TRACE_CPUPROFILER_EVENT_SCOPE(SelectVertices);
	SelectVertices(0);
	// CopySkeletalMeshToProcedural(0);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HeadShotBlendValue > 0.0f){
		//UE_LOG(LogTemp, Display, TEXT("HeadShotBlendValue In Tick : %f"), HeadShotBlendValue);
		GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("head"), HeadShotBlendValue);	
	}
	//if안에 두면 원상복구가 안됌 -> Finish가 안불림.
	HeadShotTimeline.TickTimeline(DeltaTime);
}

EEnemyState AEnemy::GetEnemyState()
{
	return EnemyState;
}

void AEnemy::Die()
{
	Tags.Add(FName("Dead"));

	OnEnemyDeath.Broadcast();

	if(BaseEnemyAIController){
		BaseEnemyAIController->SetEnemyStateAsDead();
	}else{
		UE_LOG(LogTemp, Warning, TEXT("Base Enemy AI Controller is Null"));
	}
	//Animation 때문에 바꿔야함.
	EnemyState = EEnemyState::EES_Dead;

	//Die Slice
	ApplyVertexAlphaToSkeletalMesh();
	CopySkeletalMeshToProcedural(0);
	FVector SliceNormal = FVector(0, 0, 1);  // Slice in the Z direction
	SliceMeshAtBone(SliceNormal, true);
	
	//GetMesh()->SetVisibility(false); 
	
	DisableCapsuleCollision();
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	HideHealthBar();
	//Destoryed();
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(DestoryTime);
	//Note: 랜덤적으로 Spawn하게 하면 좋을 듯 
	SpawnHealItem();
}

//쓰나? -> BPCallable인데 확인 필요
UBehaviorTree* AEnemy::GetBehaviorTree()
{
	if(!BehaviorTree){
		UE_LOG(LogTemp, Display, TEXT("Behavior Tree is None"));
	}

	return BehaviorTree;
}

UBlackboardComponent* AEnemy::GetBlackboardComponent() const 
{
	if(!BlackboardComponent){
		UE_LOG(LogTemp, Display, TEXT("BlackboardComponent is None"));
	}

	return BlackboardComponent;
}

float AEnemy::GetAttackRadius() const
{
	return AttackRadius;
}

float AEnemy::GetDefendRadius() const
{
	return DefendRadius;
}

UAnimInstance* AEnemy::GetEnemyAnimInstance() const
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	return AnimInstance;
}

void AEnemy::EnemyAutoAttack(bool bIsRandom, int32 SectionNum)
{
	UEnemyAutoAttackComponent* EnemyAutoAttackComponent = FindComponentByClass<UEnemyAutoAttackComponent>();
    if (EnemyAutoAttackComponent)
    {
        EnemyAutoAttackComponent->PlayRandomAutoAttackMontage(bIsRandom, SectionNum);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No EnemyAutoAttackComponent found! (%s) (%s)"), *GetName(), *FPaths::GetCleanFilename(__FILE__));
    }
}

void AEnemy::EnemyAOEAttack(EEnemyAOEAttackEnum AOEAttackType)
{
	UEnemyAOEAttackComponent* EnemyAOEAttackComponent = FindComponentByClass<UEnemyAOEAttackComponent>();
	if (EnemyAOEAttackComponent)
	{
		EnemyAOEAttackComponent->EnemyAOEAttack(AOEAttackType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No EnemyAOEAttackComponent found! (%s)"), *GetName(), *FPaths::GetCleanFilename(__FILE__));
	}
}

UEnemyAOEAttackComponent* AEnemy::GetEnemyAOEAttack()
{
	UEnemyAOEAttackComponent* EnemyAOEAttackComponent = FindComponentByClass<UEnemyAOEAttackComponent>();
	if (EnemyAOEAttackComponent)
	{
		return EnemyAOEAttackComponent;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No EnemyAOEAttackComponent found! (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}

void AEnemy::EnemyFireBallAttack(EEnemyFireBallEnum FireBallType)
{
	
}

void AEnemy::EnemyThrowWeaponAttack()
{
	UEnemyThrowWeaponAttackComponent* EnemyThrowWeaponAttackComponent = FindComponentByClass<UEnemyThrowWeaponAttackComponent>();
    if (EnemyThrowWeaponAttackComponent)
    {
        EnemyThrowWeaponAttackComponent->PlayThrowWeaponAttackMontage();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No EnemyThrowWeaponAttackComponent found! (%s) (%s)"), *GetName(), *FPaths::GetCleanFilename(__FILE__));
    }
}

UEnemyThrowWeaponAttackComponent* AEnemy::GetEnemyThrowWeaponAttack()
{
	UEnemyThrowWeaponAttackComponent* EnemyThrowWeaponAttackComponent = FindComponentByClass<UEnemyThrowWeaponAttackComponent>();
	if (EnemyThrowWeaponAttackComponent)
	{
		return EnemyThrowWeaponAttackComponent;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No EnemyThrowWeaponAttackComponent found! (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}

UEnemyFireBallAttackComponent* AEnemy::GetEnemyFireBall()
{
	UEnemyFireBallAttackComponent* EnemyFireBallAttackComponent = FindComponentByClass<UEnemyFireBallAttackComponent>();
	if(EnemyFireBallAttackComponent)
	{
		return EnemyFireBallAttackComponent;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No EnemyFireBallAttackComponent found! (%s)"), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}

void AEnemy::EnemyTeleport(EEnemyTeleportEnum TeleportFunctionType)
{
	UEnemyTeleportComponent* EnemyTeleportComponent = FindComponentByClass<UEnemyTeleportComponent>();
	if(EnemyTeleportComponent)
	{
		EnemyTeleportComponent->EnemyTeleportFunction(TeleportFunctionType);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No EnemyTeleportComponent found! (%s)"), *FPaths::GetCleanFilename(__FILE__));
	}
}

void AEnemy::SetAIAttackFinishDelegate(const FAIEnemyAttackFinished& InOnAttackFinished)
{
	//UE_LOG(LogTemp, Display, TEXT("SetAIAttackFinishDelegate (%s)"), *FPaths::GetCleanFilename(__FILE__));
	OnAttackFinished = InOnAttackFinished;
}

void AEnemy::AttackEnd()
{
	//UE_LOG(LogTemp, Display, TEXT("Attack End (%s)"), *FPaths::GetCleanFilename(__FILE__));
	OnAttackFinished.ExecuteIfBound();
	if(bIsPlayStunMontage){
		bIsPlayStunMontage = false;
	}
}

FVector AEnemy::GetGroundLocation(AActor* Actor)
{
    FVector StartLocation = Actor->GetActorLocation();
    FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f);
    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params))
    {
        return HitResult.Location; // 바닥 위치 반환
    }

    return StartLocation - FVector(0.f, 0.f, 20.f);
}

UHealthBar* AEnemy::GetBossHealthBar()
{
	if(BossHealthBar){
		return BossHealthBar;
	}
	else{
		UE_LOG(LogTemp, Warning, TEXT("This Actor is not Boss %s (%s)"), *GetName(), *FPaths::GetCleanFilename(__FILE__));
		return nullptr;
	}
}

void AEnemy::SetMovementSpeedEnum(EEnemyMovementSpeed NewSpeed)
{
    CurrentMovementSpeed = NewSpeed;

    // Enum에 따른 실제 이동 속도 설정
    switch (NewSpeed)
    {
        case EEnemyMovementSpeed::EEMS_Idle:
            GetCharacterMovement()->MaxWalkSpeed = IdleMovementSpeed; 
            break;

        case EEnemyMovementSpeed::EEMS_Walk:
            GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed;  
            break;

        case EEnemyMovementSpeed::EEMS_Jogging:
            GetCharacterMovement()->MaxWalkSpeed = JoggingMovementSpeed; 
            break;

        case EEnemyMovementSpeed::EEMS_Sprinting:
            GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
            break;
		case EEnemyMovementSpeed::EEMS_Teleporting:
			GetCharacterMovement()->MaxWalkSpeed = TeleportMovementSpeed;

        default:
            break;
    }
}

EEnemyMovementSpeed AEnemy::GetMovementSpeedEnum() const
{
    return CurrentMovementSpeed;
}

bool AEnemy::CanAttack()
{
    return
	!IsAttacking() && 
	!IsDead() &&
	!IsStrafing() &&
	!IsGetHitting();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if(BossHealthBar){
		BossHealthBar->SetHealthBar(Attributes->GetHealthPercent());
	}
	else if(HealthBarWidget){
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}else{
		UE_LOG(LogTemp, Warning, TEXT("can't find HealthBar Widget"));
	}
}

AActor* AEnemy::GetPatrolRoute() const
{
	return PatrollSpline;
}

void AEnemy::Healing(float HealAmount)
{
	Super::Healing(HealAmount);
	if(HealthBarWidget){
		//UE_LOG(LogTemp, Display, TEXT("Healing"));
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
		ShowHealthBar();
	}
}

void AEnemy::PlayStunMontage()
{
	if(ParryedMontage && !bIsPlayStunMontage){
		//UE_LOG(LogTemp, Display, TEXT("PlayStunMontage"));
		FName parrySection = TEXT("Default");
		ChoosePlayMontageSection(ParryedMontage, parrySection);
		bIsPlayStunMontage = true;
	}
}

void AEnemy::GetHit_Implementation(const FVector &ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	
	if(Attributes->GetHealthPercent() > 0.f){
		ShowHealthBar();
	}
	
	PlayHitSound(ImpactPoint);
	UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0) -> StartCameraShake(UVikingCameraShake::StaticClass());

	if (Hitter)
    {
        UAISense_Damage::ReportDamageEvent(
            GetWorld(),
            this,       // 데미지를 입은 액터 (AI 캐릭터)
            Hitter,     // 공격자 (플레이어 등)
            10.0f,      // 데미지 양 (필요에 따라 실제 데미지 값으로 대체)
            ImpactPoint,
            ImpactPoint - GetActorLocation()  // 데미지가 어디서 왔는지 확인가능.
        );
    }
}

void AEnemy::GetHeadShot(FVector ImpactPoint)
{
	UE_LOG(LogTemp, Display, TEXT("Enemy Head Shot"));

	if(HeadShotHitSound){
        UGameplayStatics::PlaySoundAtLocation(this, HeadShotHitSound, GetActorLocation(), 1.f, 1.f);
    }

	GetMesh()->SetAllBodiesBelowSimulatePhysics(TEXT("head"), true);
	HeadShotTimeline.PlayFromStart();

	GetWorld()->GetTimerManager().SetTimer(HeadShotImpulseDelayTimerHandle, [this, ImpactPoint]()
	{
		HeadShotAddImpulse(ImpactPoint); 
	}, 0.48f, false);
}

void AEnemy::HeadShotReaction(float Value)
{
	HeadShotBlendValue = Value;
}

void AEnemy::HeadShotAddImpulse(FVector ImpactPoint)
{
	FVector NormalizedImpact = ImpactPoint.GetSafeNormal();
	GetMesh()->AddImpulse(NormalizedImpact, TEXT("head"));
}

void AEnemy::HeadShotReactionEnd()
{
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(TEXT("head"), 0);
	GetMesh()->SetAllBodiesSimulatePhysics(false);
}

void AEnemy::SelectVertices(int32 LODIndex)
{
	if (!GetMesh() || !ProcMeshComponent){
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
        return;
    }

	// SkeletalMesh를 가져온다.
    USkeletalMesh* SkeletalMesh = GetMesh()->GetSkeletalMeshAsset();
    if (!SkeletalMesh){
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMesh is null."));
        return;
    }
	//GetResourceForRendering - Skeletal Mesh의 렌더링 데이터를 가져오는 함수
    const FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
    if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex)){
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: LODRenderData[%d] is not valid."), LODIndex);
        return;
    }
	//vertex의 총 개수를 들고온다.
	NumVertices = RenderData->LODRenderData[LODIndex].GetNumVertices();

	//SkeletalMesh에서 LODRenderData를 가져온다.LODRenderData는 버텍스 데이터, 인덱스 데이터, 섹션 정보 등이 포함
    //FSkeletalMeshLODRenderData란 LOD의 Mesh 데이터를 가지고 있는 구조체이다.
    const FSkeletalMeshLODRenderData& LODRenderData = RenderData->LODRenderData[LODIndex];
	//SkinWeightVertexBuffer를 가져온다. -> vertex가 어떤 Bone에 영향을 받는지 저장하는 데이터이며 Animation에서 사용 예정
	const FSkinWeightVertexBuffer& SkinWeights = LODRenderData.SkinWeightVertexBuffer;

	//위치를 들고온다.
	FTransform MeshTransform = GetMesh()->GetComponentTransform();
	FVector TargetBoneLocation = GetMesh()->GetBoneLocation(TargetBoneName); 

	int32 vertexCounter = 0;
    for (const FSkelMeshRenderSection& Section : LODRenderData.RenderSections){
        //NumVertices - 해당 Section의 Vertex 수, BaseVertexIndex - 해당 Section의 시작 Vertex Index
        const int32 NumSourceVertices = Section.NumVertices;
        const int32 BaseVertexIndex = Section.BaseVertexIndex;

        for (int32 i = 0; i < NumSourceVertices; i++){
            const int32 VertexIndex = i + BaseVertexIndex;

            //vertex의 위치를 가져온다. -> LODRenderData.StaticVertexBuffers.PositionVertexBuffer(현재 LOD의 Vertex 위치 데이터를 저장하는 버퍼)
            //.VertexPosition(VertexIndex)-> VertexIndex의 위치를 가져온다. 반환 타입이 FVector3f이다.
            const FVector3f SkinnedVectorPos = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);		//로컬 좌표 반환
			FVector WorldVertexPosition = MeshTransform.TransformPosition(FVector(SkinnedVectorPos)); // FVector3f -> FVector 변환 & 로컬 -> 월드 좌표
			//UE_LOG(LogTemp, Display, TEXT("WorldVertexPosition : %s"), *WorldVertexPosition.ToString());
			float DistanceToBone = FVector::Dist(WorldVertexPosition, TargetBoneLocation);
			//TargetBoneLocation을 기준으로 일정 거리 내에 있는 Vertex만 추가해서 Procedural Mesh 생성
            if (DistanceToBone <= CreateProceduralMeshDistance)
			{
				// 위치 정보 추가
				FVector LocalVertexPosition = FVector(SkinnedVectorPos);
				VertexIndexMap.Add(VertexIndex, vertexCounter);
				//WorldVertexPosition을 사용하면 다른 위치에 Procedural Mesh가 생성된다.
            	FilteredVerticesArray.Add(LocalVertexPosition);
				vertexCounter += 1;
				// 노멀(Normal), 탄젠트(Tangent) 추가
				const FVector3f Normal = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex);
				const FVector3f TangentX = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);
				const FVector2f SourceUVs = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
				Normals.Add(FVector(Normal));
				Tangents.Add(FProcMeshTangent(FVector(TangentX), false));
				UV.Add(FVector2D(SourceUVs));
				//VertexColors.Add(FColor(0, 0, 0, 255));
				VertexColors.Add(FColor(0, 0, 0, 255));
			}
        }
    }

	//UE_LOG(LogTemp, Display, TEXT("VertexIndexMan Count : %d"), VertexIndexMap.Num());
    //LODRenderData.MultiSizeIndexContainer.GetIndexBuffer()는 원래 Skeletal Mesh의 각 vertex가 어떻게 삼각형으로 구성되어있었는지를 들고온다.
    const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODRenderData.MultiSizeIndexContainer.GetIndexBuffer();
    if (!IndexBuffer){
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: Index buffer is null."));
        return;
    }

    //현재 LOD의 총 Index 수를 가져온다.
    const int32 NumIndices = IndexBuffer->Num();
	Indices.SetNum(NumIndices); // 모든 값을 0으로 초기화하며 메모리 공간 확보보
    for (int32 i = 0; i < NumIndices; i+=3){
        //IndexBuffer Get(i) - 현재 처리 중인 삼각형을 구성하는 버텍스 인덱스를 가져옴.
        //VertexIndex : Get(0) = a, Get(1) = b, Get(2) = c로 abc삼각형, Get(3) = c, Get(4) = d, Get(5) = a로 cda삼각형 (여기서 abcd는 FVector위치라고 취급)
        //즉, 첫 BaseVertexIndex에서 그려지는 삼각형부터 순서대로 삼각형이 그려지는 vertex의 Index를 가져온다.
        //결과적으로 Indices를 순환하면 3개씩 묶어서 삼각형을 그릴 수 있다.
        //uint32가 반환되어 int32로 Casting, 데이터 일관성을 위해 Casting한다.
        //Indices[i] = static_cast<int32>(IndexBuffer->Get(i));
		//아래 코드는 3각형을 이루는 3개의 i를 들고오는 것이다.
		int32 OldIndex0 = static_cast<int32>(IndexBuffer->Get(i));
		int32 OldIndex1 = static_cast<int32>(IndexBuffer->Get(i + 1));
		int32 OldIndex2 = static_cast<int32>(IndexBuffer->Get(i + 2));

		int32 NewIndex0 = VertexIndexMap.Contains(OldIndex0) ? VertexIndexMap[OldIndex0] : -1;
		int32 NewIndex1 = VertexIndexMap.Contains(OldIndex1) ? VertexIndexMap[OldIndex1] : -1;
		int32 NewIndex2 = VertexIndexMap.Contains(OldIndex2) ? VertexIndexMap[OldIndex2] : -1;
		// 기존 VertexIndex가 NewVertexIndex에 포함된 경우만 추가 - 실제로 내가 vertex 수집한 곳에 있는 Index인지 확인한다.
		//NewIndex >= FilteredVerticesArray.Num() - VertexIndexMap이 잘못된 값을 반환하거나, Indices 배열에 유효하지 않은 인덱스가 추가될 때 발생
		if (NewIndex0 < 0 || NewIndex0 >= FilteredVerticesArray.Num() || NewIndex1 < 0 || NewIndex1 >= FilteredVerticesArray.Num() || NewIndex2 < 0 || NewIndex2 >= FilteredVerticesArray.Num()){
			//UE_LOG(LogTemp, Warning, TEXT("Skipping triangle due to invalid indices: (%d, %d, %d) → (%d, %d, %d)"),	OldIndex0, OldIndex1, OldIndex2, NewIndex0, NewIndex1, NewIndex2);
		}
		else{
			Indices.Add(NewIndex0);
			Indices.Add(NewIndex1);
			Indices.Add(NewIndex2);
		}
    }

	//
	// for (int32 i = 0; i < FilteredVerticesArray.Num(); i++) {
	// 	// 잘린 부분의 버텍스를 Black(0, 0, 0)으로 설정
	// 	VertexColors.Add(FColor(0, 0, 0, 0));  // 알파 값 포함
	// }
}  

void AEnemy::ApplyVertexAlphaToSkeletalMesh()
{
    if (!GetMesh() || !GetMesh()->GetSkeletalMeshAsset()) return;

    TArray<FLinearColor> LinearVertexColors;
    LinearVertexColors.Init(FLinearColor(1, 1, 1, 1), NumVertices); // 흰색(보임)

    // VertexIndexMap을 활용해 잘린 부분만 색상을 변경
    for (const TPair<int32, int32>& Pair : VertexIndexMap) {
        int32 ColorChangeIndex = Pair.Key;  // 원본 Skeletal Mesh의 버텍스 인덱스
		if (ColorChangeIndex >= 0) {		//잘못된 Index 방지.
            LinearVertexColors[ColorChangeIndex] = FLinearColor(0, 0, 0, 0);  // 검은색 = 마스킹 처리
        }
    }

    // Skeletal Mesh에 버텍스 컬러 적용
    GetMesh()->SetVertexColorOverride_LinearColor(0, LinearVertexColors);
    GetMesh()->MarkRenderStateDirty(); // 렌더 상태 갱신
}

void AEnemy::CopySkeletalMeshToProcedural(int32 LODIndex)
{
    if (!GetMesh() || !ProcMeshComponent){
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
        return; 
    }

    //Skeletal Mesh의 Location과 Rotation을 들고온다.
    FVector MeshLocation = GetMesh()->GetComponentLocation();
    FRotator MeshRotation = GetMesh()->GetComponentRotation();

    //Skeletal Mesh의 Location과 Rotation을 Procedural Mesh에 적용한다.
    ProcMeshComponent->SetWorldLocation(MeshLocation);
    ProcMeshComponent->SetWorldRotation(MeshRotation);

    //Section Index - 어떤 Section부터 시작하는가?, Vertices - 어떤 vertex를 사용하는가?
    //Indices - 어떤 삼각형 구조를 사용하는가?, Normals, UV, Colors, Tangents, bCreateCollision - 충돌 활성화
    ProcMeshComponent->CreateMeshSection(0, FilteredVerticesArray, Indices, Normals, UV, VertexColors, Tangents, true);

    //위에선 LOD Section별로 Vertex를 가져와서 모두 처리했지만 여기서는 GetMaterial(0)로 0번째만 Material을 가져와서 적용함. 즉, 0번째 Material만 적용됨.
    //더 적용하기 위해선 수정 필요.
    UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
    if (SkeletalMeshMaterial){
        ProcMeshComponent->SetMaterial(0, SkeletalMeshMaterial);
        //UE_LOG(LogTemp, Display, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
    } else UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
}

void AEnemy::SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf) 
{
    if (!GetMesh()|| !ProcMeshComponent) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: SkeletalMeshComponent or ProcMeshComponent is null."));
        return;
    }

    FVector BoneLocation = GetMesh()->GetBoneLocation(TargetBoneName);
    if (BoneLocation == FVector::ZeroVector) {
        UE_LOG(LogTemp, Error, TEXT("SliceMeshAtBone: Failed to get Bone '%s' location. Check if the bone exists in the skeleton."), *TargetBoneName.ToString());
        return;
    }

    UMaterialInterface* ProcMeshMaterial = ProcMeshComponent->GetMaterial(0);
    if (!ProcMeshMaterial) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Procedural mesh has no material assigned."));
    }

	UProceduralMeshComponent* OtherHalfMesh = nullptr;		//잘린 Procedural Mesh가 OtherHalfMesh가 된다.
    UKismetProceduralMeshLibrary::SliceProceduralMesh(
        ProcMeshComponent,                         
        BoneLocation,                         
        SliceNormal,                          
        bCreateOtherHalf,                     
        OtherHalfMesh,                        
        EProcMeshSliceCapOption::CreateNewSectionForCap,       
        CapMaterial                           //절단면 Material
    );

	if(!OtherHalfMesh){
		UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Failed to slice mesh at bone '%s'."), *TargetBoneName.ToString());
		return;
	}
    if (ProceduralMeshAttachSocketName.IsNone() || OtherHalfMeshAttachSocketName.IsNone()) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: One or both Socket Names are invalid!"));
        return;
    }
	ProcMeshComponent->SetSimulatePhysics(false);
	OtherHalfMesh->SetSimulatePhysics(false);
	UE_LOG(LogTemp, Display, TEXT("Physic Disable"));

    //Procedural Mesh를 특정 Socket에 Attach
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ProcMeshComponent->AttachToComponent(GetMesh(), TransformRules, ProceduralMeshAttachSocketName);
	OtherHalfMesh->AttachToComponent(GetMesh(), TransformRules, OtherHalfMeshAttachSocketName);

    //Ragdoll 적용 & Bone 자름.
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->BreakConstraint(FVector(1000.f, 1000.f, 1000.f), FVector::ZeroVector, TargetBoneName);
    GetMesh()->SetSimulatePhysics(true);

    //Procedural Mesh에 물리 적용
    //ProcMeshComponent->SetSimulatePhysics(true); -> true 시 따로 움직인다.
    ProcMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	HandleDamage(DamageAmount);
	//UE_LOG(LogTemp, Display, TEXT("Take Damage"));
	
    return DamageAmount;
}


void AEnemy::HideHealthBar()
{
	if(HealthBarWidget){
		//UE_LOG(LogTemp, Display, TEXT("In Hide"));
		HealthBarWidget->SetVisibility(false);
	}
}
void AEnemy::ShowHealthBar()
{
	if(HealthBarWidget){
		HealthBarWidget->SetVisibility(true);
	}
}

bool AEnemy::IsChasing()
{
    return EnemyState == EEnemyState::EES_Chasing;
}
bool AEnemy::IsGetHitting()
{
    return EnemyState == EEnemyState::EES_Hitting;
}
bool AEnemy::IsAttacking()
{
    return EnemyState == EEnemyState::EES_Attacking;
}
bool AEnemy::IsStrafing()
{
    return EnemyState == EEnemyState::EES_Strafing;
}

bool AEnemy::IsDead()
{
    return EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::SetHitting()
{
	EnemyState = EEnemyState::EES_Hitting;
	UE_LOG(LogTemp, Display, TEXT("Set Hitting In Enemy"));
}

void AEnemy::GetHittingEnd()
{
	EnemyState = EEnemyState::EES_Strafing;
	UE_LOG(LogTemp, Display, TEXT("Get Hitting In Enemy End"));
	//CheckCombatTarget();
}

void AEnemy::BreakSkeletalBone(FVector ImpactPoint, FVector ImpactNormal, FName BreakBoneName)
{
	// if(EnemyState == EEnemyState::Dead) return;

	USkeletalMeshComponent* MeshComp = GetMesh();

	if(MeshComp){
		MeshComp->BreakConstraint(ImpactPoint, ImpactNormal * -100, BreakBoneName);
		UE_LOG(LogTemp, Warning, TEXT("Broke Constraint on Bone: %s at Location: %s"), *BreakBoneName.ToString(), *ImpactPoint.ToString());
	}else{
		UE_LOG(LogTemp, Warning, TEXT("In Break Skeletal Mesh, Mesh is nullptr"));
	}
}

void AEnemy::SpawnHealItem()
{
	UWorld* World = GetWorld();
	if(World && HealthClass){
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 10.f);
		World->SpawnActor<AHealth>(HealthClass, SpawnLocation, GetActorRotation());
	}
}