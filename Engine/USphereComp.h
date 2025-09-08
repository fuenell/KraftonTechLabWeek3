#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class USphereComp : public UPrimitiveComponent
{
private:
	// 회전도 원하면 유지
	float yaw = 0, pitch = 0, roll = 0;
	static inline std::string type = "Sphere";

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

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

	std::string GetType() const override { return type; }
};