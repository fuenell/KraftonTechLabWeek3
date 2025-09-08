#include "stdafx.h"
#include "UGizmoComponent.h"
#include "UMeshManager.h"
#include "URenderer.h"

bool UGizmoComponent::Init(UMeshManager* meshManager)
{
	if (meshManager)
	{
		mesh = meshManager->RetrieveMesh(GetType());
		return mesh != nullptr;
	}
	return false;
}

void UGizmoComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = FMatrix::SRTRowEuler(RelativeLocation, RelativeRotation, RelativeScale3D);
	renderer.SetModel(M, Color, bIsSelected);
}

void UGizmoComponent::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);
}