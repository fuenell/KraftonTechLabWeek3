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

FMatrix UGizmoComponent::GetWorldTransform()
{
	return FMatrix::SRTRowQuaternion(RelativeLocation, (OriginQuaternion * RelativeQuaternion).ToMatrixRow(), RelativeScale3D);
}

void UGizmoComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
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

void UGizmoComponent::DrawOnTop(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMeshOnTop(mesh);
}