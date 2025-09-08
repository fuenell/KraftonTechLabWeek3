#pragma once
#include "UGizmoComponent.h"

class UGizmoArrowComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	std::string GetType() const override { return "GizmoArrow"; }
};