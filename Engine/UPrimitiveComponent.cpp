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
		return Mesh != nullptr;
	}
	return false;
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