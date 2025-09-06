#include "stdafx.h"
#include "UGizmoArrowComp.h"
#include "URenderer.h"

void UGizmoArrowComp::UpdatePhysics(float t, bool bUsingGravity, float restitution)
{
	// 물리 업데이트 로직 (나중에 구현)
}

bool UGizmoArrowComp::OnCollision(UPrimitiveComponent* other, float restitution)
{
	// 충돌 처리 (나중에 구현)
	return false;
}

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