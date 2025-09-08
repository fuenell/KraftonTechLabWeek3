#pragma once
#include "UGizmoComponent.h"

class UGizmoArrowComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	std::string GetType() const override { return "GizmoArrow"; }
};