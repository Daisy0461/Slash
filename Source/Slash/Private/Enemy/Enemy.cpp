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

	CopySkeletalMeshToProcedural(0);
	FVector SliceNormal = FVector(0, 0, 1);  // Slice in the Z direction
	UMaterialInterface* CapMaterial = nullptr;  // Optionally assign a material for the cut surface
	// Call the function to slice at the bone (TargetBoneName is now a global variable)
	// SliceMeshAtBone(SliceNormal, true, CapMaterial);
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
	FVector SliceNormal = FVector(0, 0, 1);  // Slice in the Z direction
	UMaterialInterface* CapMaterial = nullptr;  // Optionally assign a material for the cut surface
	// Call the function to slice at the bone (TargetBoneName is now a global variable)
	//SliceMeshAtBone(SliceNormal, true, CapMaterial);
	//GetMesh()->SetVisibility(false);
	
	//죽은 후 Collision 없애기 -> Mesh도 없애야함.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableCapsuleCollision();
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

    // Ragdoll
    GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
    GetMesh()->SetSimulatePhysics(true);

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
	if(ParryedMontage){
		FName parrySection = TEXT("Default");
		ChoosePlayMontageSection(ParryedMontage, parrySection);
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

void AEnemy::CopySkeletalMeshToProcedural(int32 LODIndex)
{
    if (!GetMesh() || !ProcMeshComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
        return;
    }

    //Skeletal Mesh의 Location과 Rotation을 들고온다.
    FVector MeshLocation = GetMesh()->GetComponentLocation();
    FRotator MeshRotation = GetMesh()->GetComponentRotation();

    //Skeletal Mesh의 Location과 Rotation을 Procedural Mesh에 적용한다.
    ProcMeshComponent->SetWorldLocation(MeshLocation);
    ProcMeshComponent->SetWorldRotation(MeshRotation);

    // SkeletalMesh를 가져온다.
    USkeletalMesh* SkeletalMesh = GetMesh()->GetSkeletalMeshAsset();
    if (!SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMesh is null."));
        return;
    }
    //GetResourceForRendering - Skeletal Mesh의 렌더링 데이터를 가져오는 함수
    const FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
    if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: LODRenderData[%d] is not valid."), LODIndex);
        return;
    }
    //SkeletalMesh에서 LODRenderData를 가져온다.LODRenderData는 버텍스 데이터, 인덱스 데이터, 섹션 정보 등이 포함
    //FSkeletalMeshLODRenderData란 LOD의 Mesh 데이터를 가지고 있는 구조체이다.
    const FSkeletalMeshLODRenderData& LODRenderData = RenderData->LODRenderData[LODIndex];

    //SkinWeightVertexBuffer를 가져온다. -> vertex가 어떤 Bone에 영향을 받는지 저장하는 데이터이며 Animation에서 사용 예정
    const FSkinWeightVertexBuffer& SkinWeights = LODRenderData.SkinWeightVertexBuffer;
    //UE_LOG(LogTemp, Display, TEXT("CopySkeletalMeshToProcedural: Processing LODIndex %d."), LODIndex);

    TArray<FVector> VerticesArray;      //vertex위치 정보를 저장하는 TArray
    TArray<FVector> Normals;
    TArray<FVector2D> UV;
    TArray<FColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    int32 TotalVertices = 0;
    for (const FSkelMeshRenderSection& Section : LODRenderData.RenderSections)
    {
        //NumVertices - 해당 Section의 Vertex 수, BaseVertexIndex - 해당 Section의 시작 Vertex Index
        const int32 NumSourceVertices = Section.NumVertices;
        const int32 BaseVertexIndex = Section.BaseVertexIndex;

        //UE_LOG(LogTemp, Display, TEXT("Processing %d vertices in section... (BaseVertexIndex: %d)"), NumSourceVertices, BaseVertexIndex);

        for (int32 i = 0; i < NumSourceVertices; i++)
        {
            const int32 VertexIndex = i + BaseVertexIndex;

            //vertex의 위치를 가져온다. -> LODRenderData.StaticVertexBuffers.PositionVertexBuffer(현재 LOD의 Vertex 위치 데이터를 저장하는 버퍼)
            //.VertexPosition(VertexIndex)-> VertexIndex의 위치를 가져온다. 반환 타입이 FVector3f이다.
            const FVector3f SkinnedVectorPos = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
            //FVector3f -> FVector로 변환 후 VerticesArray에 추가한다.
            VerticesArray.Add(FVector(SkinnedVectorPos));

            //VertexIndex에 해당하는 버텍스의 노멀(Normal) 벡터를 반환.
            const FVector3f Normal = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex);
            //VertexIndex에 해당하는 버텍스의 탄젠트(Tangent) 벡터를 반환. - 노멀 벡터와 함께 사용되어 표면의 방향과 비틀림을 결정하는 값
            const FVector3f TangentX = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);
            //VertexIndex에 해당하는 버텍스의 UV 좌표를 반환. - 메쉬의 특정 버텍스가 텍스처에서 어디에 매핑되는지를 결정하는 2D 좌표
            const FVector2f SourceUVs = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
            
            //이후 Procedural Mesh를 생성할 때 사용하기 위해 Array에 추가.
            Normals.Add(FVector(Normal));
            Tangents.Add(FProcMeshTangent(FVector(TangentX), false));
            UV.Add(FVector2D(SourceUVs));
            Colors.Add(FColor(0, 0, 0, 255));
        }

        TotalVertices += NumSourceVertices;
    }

    UE_LOG(LogTemp, Display, TEXT("Total vertices processed: %d"), TotalVertices);

    //LODRenderData.MultiSizeIndexContainer.GetIndexBuffer()는 원래 Skeletal Mesh의 각 vertex가 어떻게 삼각형으로 구성되어있었는지를 들고온다.
    const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODRenderData.MultiSizeIndexContainer.GetIndexBuffer();
    if (!IndexBuffer)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: Index buffer is null."));
        return;
    }

    //현재 LOD의 총 Index 수를 가져온다.
    const int32 NumIndices = IndexBuffer->Num();

    TArray<int32> Indices;
    //메모리 미리 확보
    Indices.SetNumUninitialized(NumIndices);
    for (int32 i = 0; i < NumIndices; i++)
    {
        //IndexBuffer Get(i) - 현재 처리 중인 삼각형을 구성하는 버텍스 인덱스를 가져옴.
        //VertexIndex : Get(0) = a, Get(1) = b, Get(2) = c로 abc삼각형, Get(3) = c, Get(4) = d, Get(5) = a로 cda삼각형 (여기서 abcd는 FVector위치라고 취급)
        //즉, 첫 BaseVertexIndex에서 그려지는 삼각형부터 순서대로 삼각형이 그려지는 vertex의 Index를 가져온다.
        //결과적으로 Indices를 순환하면 3개씩 묶어서 삼각형을 그릴 수 있다.
        //uint32가 반환되어 int32로 Casting, 데이터 일관성을 위해 Casting한다.
        Indices[i] = static_cast<int32>(IndexBuffer->Get(i));
    }

    // Create procedural mesh section
    //Section Index - 어떤 Section부터 시작하는가?, Vertices - 어떤 vertex를 사용하는가?
    //Indices - 어떤 삼각형 구조를 사용하는가?, Normals, UV, Colors, Tangents, bCreateCollision - 충돌 활성화
    ProcMeshComponent->CreateMeshSection(0, VerticesArray, Indices, Normals, UV, Colors, Tangents, true);
    UE_LOG(LogTemp, Display, TEXT("Procedural mesh creation completed."));

    //Convex Collision 추가
    if (VerticesArray.Num() > 0)
    {
        ProcMeshComponent->ClearCollisionConvexMeshes();  // 기존 Collision 삭제
        //Convex Collision - 현재 Vertex 기반으로 Convex(볼록한) Collision 생성
        ProcMeshComponent->AddCollisionConvexMesh(VerticesArray);  // Convex Collision 추가
        UE_LOG(LogTemp, Display, TEXT("Convex Collision added with %d vertices."), VerticesArray.Num());
    }

    // Collision 및 Physics 설정
    ProcMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ProcMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
    ProcMeshComponent->SetSimulatePhysics(true);
    ProcMeshComponent->SetEnableGravity(true);

    //위에선 LOD Section별로 Vertex를 가져와서 모두 처리했지만 여기서는 GetMaterial(0)로 0번째만 Material을 가져와서 적용함. 즉, 0번째 Material만 적용됨.
    //더 적용하기 위해선 수정 필요.
    UMaterialInterface* SkeletalMeshMaterial = GetMesh()->GetMaterial(0);
    if (SkeletalMeshMaterial)
    {
        ProcMeshComponent->SetMaterial(0, SkeletalMeshMaterial);
        UE_LOG(LogTemp, Display, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
    }
}

void AEnemy::SliceMeshAtBone(FVector SliceNormal, bool bCreateOtherHalf, UMaterialInterface* CapMaterial) 
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

    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Target Bone '%s' is at (%f, %f, %f)"), *TargetBoneName.ToString(), BoneLocation.X, BoneLocation.Y, BoneLocation.Z);
    
    UMaterialInterface* ProcMeshMaterial = ProcMeshComponent->GetMaterial(0);
    if (!ProcMeshMaterial) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Procedural mesh has no material assigned."));
    }

    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Starting slice operation..."));

    UProceduralMeshComponent* OtherHalfMesh = nullptr;
    UKismetProceduralMeshLibrary::SliceProceduralMesh(
        ProcMeshComponent,                         
        BoneLocation,                         
        SliceNormal,                          
        bCreateOtherHalf,                     
        OtherHalfMesh,                        
        EProcMeshSliceCapOption::NoCap,       
        CapMaterial                           
    );

    ProcMeshComponent->AddImpulse(FVector(1000.f, 1000.f, 1000.f), NAME_None, true);
    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Impulse added to ProceduralMesh."));
    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Mesh slicing process finished."));
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