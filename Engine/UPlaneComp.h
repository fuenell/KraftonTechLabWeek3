#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "Vector.h"

class UPlaneComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UPlaneComp, UPrimitiveComponent)
public:
	using UPrimitiveComponent::UPrimitiveComponent;
};