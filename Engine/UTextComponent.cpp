#include "stdafx.h"
#include "UTextComponent.h"
#include "URenderer.h"

IMPLEMENT_UCLASS(UTextComponent, USceneComponent)

void UTextComponent::Draw(URenderer& renderer)
{
	if (!TMesh || !TMesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawTextMesh(TMesh);
}
void UTextComponent::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = GetWorldTransform();
	renderer.SetModel(M, Color, bIsSelected);
}