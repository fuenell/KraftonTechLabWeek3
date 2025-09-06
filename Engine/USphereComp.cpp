#include "stdafx.h"
#include "USphereComp.h"
#define JSON_HEADER
#include "json.hpp"

void USphereComp::UpdatePhysics(float t, bool bUsingGravity, float restitution)
{
    // 물리 업데이트 로직 (나중에 구현)
}

bool USphereComp::OnCollision(UPrimitiveComponent* other, float restitution)
{
    // 충돌 처리 (나중에 구현)
    return false;
}

void USphereComp::UpdateConstantBuffer(URenderer& renderer)
{
    FMatrix M = FMatrix::TRSRow(RelativeLocation, FVector(pitch, yaw, roll), RelativeScale3D);
    renderer.SetModel(M);          // M,V,P를 통째로 상수버퍼에 업로드
}

void USphereComp::Draw(URenderer& renderer)
{
    if (!mesh || !mesh->VertexBuffer)
    {
        return;
    }

    UpdateConstantBuffer(renderer);
    renderer.DrawMesh(mesh);
}