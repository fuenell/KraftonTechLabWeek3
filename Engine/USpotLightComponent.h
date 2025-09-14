#pragma once
#include "UPrimitiveComponent.h"


class USpotLightComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(USpotLightComponent, UPrimitiveComponent)

public:
	USpotLightComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }) :UPrimitiveComponent(pos, rot, scl)
	{
		Color = { 0, 0, 0, 0 };
	}
	~USpotLightComponent();


private:
	bool IsManageable() override { return true; }

private:
	float Angle = 45.0f;
	float Scale = 20.0f;
};

