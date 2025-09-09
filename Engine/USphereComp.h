#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class USphereComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(USphereComp, UPrimitiveComponent)
private:
	static inline FString type = "Sphere";

	static USceneComponent* Create()
	{
		USceneComponent* newInstance = new USphereComp();

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
	USphereComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
	}

	FString GetType() const override { return type; }
};