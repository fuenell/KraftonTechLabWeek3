#pragma once
#include "UGizmoComponent.h"
#include "UGizmoManager.h"

class UGizmoScaleHandleComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	EAxis Axis = EAxis::None;

	std::string GetType() const override { return "GizmoScaleHandle"; }
};