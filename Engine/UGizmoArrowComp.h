#pragma once
#include "UPrimitiveComponent.h"

class UGizmoArrowComp : public UPrimitiveComponent
{
public:
	using UPrimitiveComponent::UPrimitiveComponent;

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

	void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	std::string GetType() const override { return "GizmoArrow"; }
};