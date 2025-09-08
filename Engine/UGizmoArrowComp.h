#pragma once
#include "UGizmoComponent.h"

class UGizmoArrowComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	EAxis Axis = EAxis::None;

	std::string GetType() const override { return "GizmoArrow"; }
};