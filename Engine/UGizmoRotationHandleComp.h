#pragma once
#include "UGizmoComponent.h"
#include "UGizmoManager.h"

class UGizmoRotationHandleComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	EAxis Axis = EAxis::None;

	std::string GetType() const override { return "GizmoRotationHandle"; }
};