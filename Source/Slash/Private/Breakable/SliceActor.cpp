// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/SliceActor.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshResources.h"

// Sets default values
ASliceActor::ASliceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProcMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
	ProcMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASliceActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ASliceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASliceActor::CopyFromStaticMesh()
{
    if (!StaticMeshComponent || !StaticMeshComponent->GetStaticMesh())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid StaticMeshComponent or missing StaticMesh."));
        return;
    } 

    UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
    FStaticMeshLODResources& LODResource = StaticMesh->GetRenderData()->LODResources[0];

    const FPositionVertexBuffer& VertexBuffer = LODResource.VertexBuffers.PositionVertexBuffer;
    const FIndexArrayView IndexBuffer = LODResource.IndexBuffer.GetArrayView();
    const FStaticMeshVertexBuffer& StaticMeshVertexBuffer = LODResource.VertexBuffers.StaticMeshVertexBuffer;

    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FProcMeshTangent> Tangents;

    // ✅ 1. 정점(Vertex) 데이터 복사
    uint32 VertexCount = VertexBuffer.GetNumVertices();
    for (uint32 i = 0; i < VertexCount; i++)
    {
        Vertices.Add(FVector(VertexBuffer.VertexPosition(i))); // ✅ FVector3f → FVector 변환
    }

    // ✅ 2. 인덱스(Index) 데이터 복사
    for (int32 i = 0; i < IndexBuffer.Num(); i++)
    {
        Triangles.Add(IndexBuffer[i]);
    }

    // ✅ 3. 노멀(Normal) 데이터 복사
    for (uint32 i = 0; i < VertexCount; i++)
    {
        Normals.Add(FVector(StaticMeshVertexBuffer.VertexTangentZ(i))); // ✅ FVector3f → FVector 변환
    }

    // ✅ 4. UV 데이터 복사 (채널 0)
    if (StaticMeshVertexBuffer.GetNumTexCoords() > 0)
    {
        for (uint32 i = 0; i < VertexCount; i++)
        {
            UVs.Add(FVector2D(StaticMeshVertexBuffer.GetVertexUV(i, 0))); // ✅ FVector2f → FVector2D 변환
        }
    }

    // ✅ 5. 탠전(Tangent) 데이터 복사
    for (uint32 i = 0; i < VertexCount; i++)
    {
        Tangents.Add(FProcMeshTangent(FVector(StaticMeshVertexBuffer.VertexTangentX(i)), false)); // ✅ FVector3f → FVector 변환
    }

    // ✅ 6. Procedural Mesh로 설정
    ProcMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, TArray<FLinearColor>(), Tangents, true);

    // ✅ 7. Collision 활성화
    ProcMeshComponent->ContainsPhysicsTriMeshData(true);
}
