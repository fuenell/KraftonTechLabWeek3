#pragma once
#include "UGizmoComponent.h"
#include "UGizmoManager.h"

class UGizmoArrowComp : public UGizmoComponent
{
public:
	using UGizmoComponent::UGizmoComponent;

	EAxis Axis = EAxis::None;

	FString GetType() const override { return "GizmoArrow"; }
};