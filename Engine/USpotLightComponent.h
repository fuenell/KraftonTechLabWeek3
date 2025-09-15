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

	// getter
	inline float GetAngle() const { return Angle; }
	inline float GetScale() const { return Scale; }
	inline FVector4 GetLightColor() const { return LightColor; }

	// setter
	inline void  SetAngle(float InAngle) { Angle = InAngle; }
	inline void  SetScale(float InScale) { Scale = InScale; }
	inline void	 SetLightColor(const FVector4& InLightColor) { LightColor = InLightColor; }

private:
	bool IsManageable() override { return true; }

private:
	FVector4 LightColor = {};
	float Angle = 45.0f;
	float Scale = 20.0f;
};

