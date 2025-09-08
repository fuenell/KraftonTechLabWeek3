#include "stdafx.h"
#include "UGizmoGridComp.h"
#include "URenderer.h"

void UGizmoGridComp::UpdateConstantBuffer(URenderer& renderer)
{
	FMatrix M = FMatrix::TRSRow(RelativeLocation, FVector(0, 0, 0), RelativeScale3D);
	renderer.SetModel(M, bIsSelected);          // M,V,P를 통째로 상수버퍼에 업로드
}

void UGizmoGridComp::Draw(URenderer& renderer)
{
	if (!mesh || !mesh->VertexBuffer)
	{
		return;
	}

	UpdateConstantBuffer(renderer);
	renderer.DrawMesh(mesh);
}