#include "stdafx.h"
#include "UGizmoArrowComp.h"
#include "URenderer.h"

void UGizmoArrowComp::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = FMatrix::TRSRow(RelativeLocation, RelativeRotation, RelativeScale3D);
	renderer.SetModel(M, bIsSelected);          // M,V,P를 통째로 상수버퍼에 업로드
}

void UGizmoArrowComp::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);
}