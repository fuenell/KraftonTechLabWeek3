#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class json::JSON;

class UGizmoGridComp : public UPrimitiveComponent
{
public:
	UGizmoGridComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
	}

	json::JSON Serialize() const override;

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

    void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	FVector GetPosition() const { return RelativeLocation; }
	FVector GetScale() const { return RelativeScale3D; }

	std::string GetType() const override { return "GizmoGrid"; }
};