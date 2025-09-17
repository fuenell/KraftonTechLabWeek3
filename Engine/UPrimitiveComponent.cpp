#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
bool UPrimitiveComponent::Init(UMeshManager* meshManager)
{
	if (meshManager)
	{
		Mesh = meshManager->RetrieveMesh(GetClass()->GetUClassName());
		BoundingBox = Mesh->GetLocalBounds();
		return Mesh != nullptr;
	}
	return false;
}

void UPrimitiveComponent::UpdateAccurateBounds(const FMatrix& WorldTransform)
{
	// 이전에 이미 계산을 수행했고, 파라미터로 들어온 WorldTransform이 마지막으로 사용된 것과 같다면
	if (LastAccurateBoundsTransform == WorldTransform)
	{
		return;
	}

	LastAccurateBoundsTransform = WorldTransform;

	// 원본 정점 데이터의 '사본'을 만듭니다
	TArray<FVertexPosColor4> WorldVertices = Mesh->Vertices;

	// 사본 배열의 모든 정점을 하나씩 순회하며 월드 좌표로 변환합니다.
	for (FVertexPosColor4& Vertex : WorldVertices)
	{
		// 로컬 좌표를 월드 좌표로 변환
		FVector NewPosition = FMatrix::MultiplyVectorRow(Vertex.GetPosition(), WorldTransform).ToVec3Homogenized();

		// 변환된 좌표로 갱신
		Vertex.SetPosition(NewPosition);
	}

	// 변환된 '모든' 월드 좌표 정점들을 기반으로 딱 맞는 새로운 AABB를 계산합니다.
	const FBounds& AccurateWorldBounds = UMesh::ComputeLocalBounds(WorldVertices);

	BoundingBox = AccurateWorldBounds;
}

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected);
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!Mesh || !Mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(Mesh);
}