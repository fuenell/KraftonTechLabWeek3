#include "stdafx.h"
#include "USphereComp.h"

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
    // row 규약: M = S * R * T
    FMatrix S = FMatrix::Scale(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z); // 대각행렬 동일
    FMatrix Rx = FMatrix::RotationXRow(pitch);
    FMatrix Ry = FMatrix::RotationYRow(yaw);
    FMatrix Rz = FMatrix::RotationZRow(roll);
    FMatrix R = Rz * (Ry * Rx);   // 원하는 회전 순서로
    FMatrix T = FMatrix::TranslationRow(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);

    FMatrix M = S * R * T;
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