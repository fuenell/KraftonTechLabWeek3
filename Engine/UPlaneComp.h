#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "Vector.h"

class UPlaneComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UPlaneComp, UPrimitiveComponent)
private:
	static inline FString type = "Plane";
	FVector trianglePoints;

	static USceneComponent* Create()
	{
		USceneComponent* newInstance = new UPlaneComp();

		return newInstance;
	}

	static bool RegisterToFactory()
	{
		static bool isRegistered = false;
		if (isRegistered) return true;

		USceneComponentFactory::Register(type, Create);
		isRegistered = true;
		return true;
	}

	static inline bool registered = RegisterToFactory();

	bool IsManageable() override { return true; }

public:
	UPlaneComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
	}

	FString GetType() const override { return type; }
};