#include "Enemy/Enemy.h"
#include "Enemy/BaseEnemyAIController.h"
#include "Character/VikingCameraShake.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h" 
//Attribute는 삭제 예정 - HealthWidget이 있음.
#include "Components/AttributeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "ProceduralMeshComponent.h"
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

#include "KismetProceduralMeshLibrary.h"  

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

	CopySkeletalMeshToProcedural(GetMesh(), 0, ProcMeshComponent);

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
	Super::Die();
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
	SliceMeshAtBone(GetMesh(), ProcMeshComponent, SliceNormal, true, CapMaterial);
	GetMesh()->SetVisibility(false);
	
	//죽은 후 Collision 없애기 -> Mesh도 없애야함.
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DisableCapsuleCollision();
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	
	//SetWeaponCollision(ECollisionEnabled::NoCollision);
	HideHealthBar();
	//Destoryed();
	//죽은 후 일정시간 후 Destroy
	SetLifeSpan(DestoryTime);
	GetCharacterMovement()->bOrientRotationToMovement = false;

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

void AEnemy::CopySkeletalMeshToProcedural(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComp)
{
    if (!SkeletalMeshComponent || !ProcMeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMeshComponent or ProcMeshComp is null."));
        return;
    }

    // Get SkeletalMesh Location and Rotation
    FVector MeshLocation = SkeletalMeshComponent->GetComponentLocation();
    FRotator MeshRotation = SkeletalMeshComponent->GetComponentRotation();

    // Apply Transform to Procedural Mesh
    ProcMeshComp->SetWorldLocation(MeshLocation);
    ProcMeshComp->SetWorldRotation(MeshRotation);

    UE_LOG(LogTemp, Log, TEXT("Procedural Mesh Transform Set -> Location: (%f, %f, %f), Rotation: (%f, %f, %f)"),
        MeshLocation.X, MeshLocation.Y, MeshLocation.Z,
        MeshRotation.Pitch, MeshRotation.Yaw, MeshRotation.Roll);

    // Get SkeletalMesh
    USkeletalMesh* SkeletalMesh = SkeletalMeshComponent->GetSkeletalMeshAsset();
    if (!SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: SkeletalMesh is null."));
        return;
    }

    // Get FSkeletalMeshRenderData
    const FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();
    if (!RenderData || !RenderData->LODRenderData.IsValidIndex(LODIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: LODRenderData[%d] is not valid."), LODIndex);
        return;
    }

    const FSkeletalMeshLODRenderData& LODRenderData = RenderData->LODRenderData[LODIndex];

    // Get Skin Weight
    const FSkinWeightVertexBuffer& SkinWeights = LODRenderData.SkinWeightVertexBuffer;

    UE_LOG(LogTemp, Log, TEXT("CopySkeletalMeshToProcedural: Processing LODIndex %d."), LODIndex);

    TArray<FVector> VerticesArray;
    TArray<FVector> Normals;
    TArray<FVector2D> UV;
    TArray<FColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    int32 TotalVertices = 0;
    for (const FSkelMeshRenderSection& Section : LODRenderData.RenderSections)
    {
        const int32 NumSourceVertices = Section.NumVertices;
        const int32 BaseVertexIndex = Section.BaseVertexIndex;

        UE_LOG(LogTemp, Log, TEXT("Processing %d vertices in section... (BaseVertexIndex: %d)"), NumSourceVertices, BaseVertexIndex);

        for (int32 i = 0; i < NumSourceVertices; i++)
        {
            const int32 VertexIndex = i + BaseVertexIndex;

            // Get vertex position
            const FVector3f SkinnedVectorPos = LODRenderData.StaticVertexBuffers.PositionVertexBuffer.VertexPosition(VertexIndex);
            VerticesArray.Add(FVector(SkinnedVectorPos));

            // Get normals and tangents
            const FVector3f Normal = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(VertexIndex);
            const FVector3f TangentX = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(VertexIndex);

            Normals.Add(FVector(Normal));
            Tangents.Add(FProcMeshTangent(FVector(TangentX), false));

            // Get UV coordinates
            const FVector2f SourceUVs = LODRenderData.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, 0);
            UV.Add(FVector2D(SourceUVs));

            // Default color (black)
            Colors.Add(FColor(0, 0, 0, 255));

            if (i % 100 == 0)  // Log every 100 vertices to prevent spam
            {
                UE_LOG(LogTemp, Log, TEXT("Vertex %d: Position (%f, %f, %f), UV (%f, %f)"),
                    VertexIndex, SkinnedVectorPos.X, SkinnedVectorPos.Y, SkinnedVectorPos.Z, SourceUVs.X, SourceUVs.Y);
            }
        }

        TotalVertices += NumSourceVertices;
    }

    UE_LOG(LogTemp, Log, TEXT("Total vertices processed: %d"), TotalVertices);

    // Get index buffer
    const FRawStaticIndexBuffer16or32Interface* IndexBuffer = LODRenderData.MultiSizeIndexContainer.GetIndexBuffer();
    if (!IndexBuffer)
    {
        UE_LOG(LogTemp, Warning, TEXT("CopySkeletalMeshToProcedural: Index buffer is null."));
        return;
    }

    // Get number of indices
    const int32 NumIndices = IndexBuffer->Num();
    UE_LOG(LogTemp, Log, TEXT("Loading %d indices..."), NumIndices);

    // Convert uint32 to int32
    TArray<int32> Indices;
    Indices.SetNumUninitialized(NumIndices);
    for (int32 i = 0; i < NumIndices; i++)
    {
        Indices[i] = static_cast<int32>(IndexBuffer->Get(i));

        if (i % 300 == 0)  // Log every 300 indices to prevent spam
        {
            UE_LOG(LogTemp, Log, TEXT("Index %d: %d"), i, Indices[i]);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Index loading completed. Total indices: %d"), Indices.Num());

    // Create procedural mesh section
    UE_LOG(LogTemp, Log, TEXT("Creating procedural mesh section..."));
    ProcMeshComp->CreateMeshSection(0, VerticesArray, Indices, Normals, UV, Colors, Tangents, true);
    UE_LOG(LogTemp, Log, TEXT("Procedural mesh creation completed."));

    // **Apply Material from SkeletalMesh**
    UMaterialInterface* SkeletalMeshMaterial = SkeletalMeshComponent->GetMaterial(0);
    if (SkeletalMeshMaterial)
    {
        ProcMeshComp->SetMaterial(0, SkeletalMeshMaterial);
        UE_LOG(LogTemp, Log, TEXT("Applied material from SkeletalMesh to ProceduralMesh."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh has no material assigned."));
    }
}

void AEnemy::SliceMeshAtBone(USkeletalMeshComponent* SkeletalMeshComponent, UProceduralMeshComponent* ProcMeshComp, FVector SliceNormal, bool bCreateOtherHalf, UMaterialInterface* CapMaterial) {
    if (!SkeletalMeshComponent || !ProcMeshComp) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: SkeletalMeshComponent or ProcMeshComp is null."));
        return;
    }

    FVector BoneLocation = SkeletalMeshComponent->GetBoneLocation(TargetBoneName);
    if (BoneLocation == FVector::ZeroVector) {
        UE_LOG(LogTemp, Error, TEXT("SliceMeshAtBone: Failed to get Bone '%s' location. Check if the bone exists in the skeleton."), *TargetBoneName.ToString());
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Target Bone '%s' is at (%f, %f, %f)"), *TargetBoneName.ToString(), BoneLocation.X, BoneLocation.Y, BoneLocation.Z);
    
    UMaterialInterface* ProcMeshMaterial = ProcMeshComp->GetMaterial(0);
    if (!ProcMeshMaterial) {
        UE_LOG(LogTemp, Warning, TEXT("SliceMeshAtBone: Procedural mesh has no material assigned."));
    }

    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Starting slice operation..."));

    // ✅ 올바른 인수 개수 (7개) 맞추고 `OtherHalfMesh` 추가
    UProceduralMeshComponent* OtherHalfMesh = nullptr;
    UKismetProceduralMeshLibrary::SliceProceduralMesh(
        ProcMeshComp,                         // 자를 Procedural Mesh
        BoneLocation,                         // Bone 위치를 기준으로 자르기
        SliceNormal,                          // 절단 방향
        bCreateOtherHalf,                     // 다른 절반 생성 여부
        OtherHalfMesh,                        // ✅ 5번째 인수: 잘린 조각을 받을 변수 추가
        EProcMeshSliceCapOption::NoCap,       // ✅ 6번째 인수: Cap Option (NoCap / UseLastMaterial 가능)
        CapMaterial                           // ✅ 7번째 인수: 단면에 적용할 머티리얼
    );

    UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Slice operation completed successfully."));

    // 잘린 후 머티리얼 유지
    ProcMeshComp->SetMaterial(0, ProcMeshMaterial);
    if (OtherHalfMesh) {
        OtherHalfMesh->SetMaterial(0, ProcMeshMaterial);
        UE_LOG(LogTemp, Display, TEXT("SliceMeshAtBone: Applied material to both halves."));
    }

    ProcMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    if (OtherHalfMesh) {
        OtherHalfMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

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