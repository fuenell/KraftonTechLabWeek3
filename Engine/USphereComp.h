﻿#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class USphereComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(USphereComp, UPrimitiveComponent)
private:
	bool IsManageable() override { return true; }
public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
		Type = PrimitiveType::Sphere;
	}
};