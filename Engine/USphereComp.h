#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
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

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

    void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }

	float GetRadius()
	{
		// TODO: account for non-uniform scaling
		return RelativeScale3D.X;
	}
};