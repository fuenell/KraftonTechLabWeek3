#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexSimple.h"
#include "Vector.h"

class USphereComp : public UPrimitiveComponent
{
private:
	// 회전도 원하면 유지
    float   yaw = 0, pitch = 0, roll = 0;


public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }, UMesh* sphereMesh = nullptr)
	{
		mesh = sphereMesh;
		RelativeLocation = pos;
		RelativeScale3D = scl;
	}

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override
	{
		// 물리 업데이트 로직 (나중에 구현)
	}

	bool OnCollision(UPrimitiveComponent* other, float restitution) override
	{
		// 충돌 처리 (나중에 구현)
		return false;
	}

    void UpdateConstantBuffer(URenderer& renderer) override
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

	void Draw(URenderer& renderer) override
	{
		if (!mesh || !mesh->VertexBuffer)
		{
			return;
		}

		UpdateConstantBuffer(renderer);
		renderer.DrawMesh(mesh);
	}

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }
};