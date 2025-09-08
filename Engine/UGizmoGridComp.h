#pragma once
#include "stdafx.h"
#include "UGizmoComponent.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class URenderer;

class UGizmoGridComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	void UpdateConstantBuffer(URenderer& renderer) override;
	void Draw(URenderer& renderer) override;

	std::string GetType() const override { return "GizmoGrid"; }
};