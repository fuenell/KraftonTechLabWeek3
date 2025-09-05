#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitive.h"
#include "FVertexSimple.h"
#include "Vector.h"
class USphere : public UPrimitive
{
private:
    FVector position;
    FVector scale;

    // 회전도 원하면 유지
    float   yaw = 0, pitch = 0, roll = 0;


public:
    USphere(FVector pos = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }, UMesh* sphereMesh = nullptr)
        : position(pos), scale(scl)
    {
        mesh = sphereMesh;
    }

    void UpdatePhysics(float t, bool bUsingGravity, float restitution) override
    {
        // 물리 업데이트 로직 (나중에 구현)
    }

    bool OnCollision(UPrimitive* other, float restitution) override
    {
        // 충돌 처리 (나중에 구현)
        return false;
    }

    void UpdateConstantBuffer(URenderer& renderer) override
    {
        // row 규약: M = S * R * T
        FMatrix S = FMatrix::Scale(scale.X, scale.Y, scale.Z); // 대각행렬 동일
        FMatrix Rx = FMatrix::RotationXRow(pitch);
        FMatrix Ry = FMatrix::RotationYRow(yaw);
        FMatrix Rz = FMatrix::RotationZRow(roll);
        FMatrix R = Rz * (Ry * Rx);   // 원하는 회전 순서로
        FMatrix T = FMatrix::TranslationRow(position.X, position.Y, position.Z);

        FMatrix M = S * R * T;
        renderer.SetModel(M);          // M,V,P를 통째로 상수버퍼에 업로드

    }

    void Draw(URenderer& renderer) override
    {
        if (!mesh || !mesh->VertexBuffer) {
            return;
        }
        // 이 줄이 누락되었습니다!
        UpdateConstantBuffer(renderer);
        renderer.DrawMesh(mesh);
    }

    // 위치와 스케일 설정 함수들
    void SetPosition(const FVector& pos) { position = pos; }
    void SetScale(const FVector& scl) { scale = scl; }
    FVector GetPosition() const { return position; }
    FVector GetScale() const { return scale; }
};