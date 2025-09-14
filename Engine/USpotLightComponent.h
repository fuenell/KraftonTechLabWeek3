#pragma once
#include "UPrimitiveComponent.h"


class USpotLightComponent : public UPrimitiveComponent
{
	DECLARE_UCLASS(USpotLightComponent, UPrimitiveComponent)


public:
	USpotLightComponent(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 }) :UPrimitiveComponent(pos, rot, scl)
	{
		Type = PrimitiveType::Cube;
	}
	~USpotLightComponent();

private:
	float Angle = 45.0f;
	float Scale = 20.0f;


};

