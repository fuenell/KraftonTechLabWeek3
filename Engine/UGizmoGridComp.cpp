#include "stdafx.h"
#include "UGizmoGridComp.h"

json::JSON UGizmoGridComp::Serialize() const
{
    return json::JSON();
}

void UGizmoGridComp::UpdatePhysics(float t, bool bUsingGravity, float restitution)
{
    // Gizmo Grid는 물리 시뮬레이션이 필요 없음
    // 필요시 구현
}

bool UGizmoGridComp::OnCollision(UPrimitiveComponent* other, float restitution)
{
    // Gizmo Grid는 충돌 처리가 필요 없음
    return false;
}

void UGizmoGridComp::UpdateConstantBuffer(URenderer& renderer)
{
    FMatrix M = FMatrix::TRSRow(RelativeLocation, FVector(0,0,0), RelativeScale3D);
    renderer.SetModel(M);          // M,V,P를 통째로 상수버퍼에 업로드
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