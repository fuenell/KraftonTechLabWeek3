#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"

IMPLEMENT_UCLASS(UPrimitiveComponent, USceneComponent)
bool UPrimitiveComponent::Init(UMeshManager* meshManager)
{
	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetType());
		return mesh != nullptr;
	}
	return false;
}

void UPrimitiveComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = FMatrix::SRTRowEuler(RelativeLocation, RelativeRotation, RelativeScale3D);
	renderer.SetModel(M, Color, bIsSelected);
}

void UPrimitiveComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);
}