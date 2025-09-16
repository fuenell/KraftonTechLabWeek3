#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "Vector.h"

class USubUVComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(USubUVComponent, UPrimitiveComponent)
private:
	bool IsManageable() override { return true; }
public:
	USubUVComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
	}
};