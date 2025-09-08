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

	static USceneComponent* Create(json::JSON data, uint32 uuid)
	{
		USceneComponent* newInstance = new USphereComp(uuid);
		newInstance->Deserialize(data);
		newInstance->SetUUID(uuid);

		return newInstance;
	}

	void RegisterToFactory()
	{
		static bool isRegistered = false;
		if (isRegistered) return;

		USceneComponentFactory::Register(GetType(), Create);
		isRegistered = true;  // 플래그 설정
	}

public:
	USphereComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
		RegisterToFactory();
	}

	void UpdatePhysics(float t, bool bUsingGravity, float restitution) override;
	bool OnCollision(UPrimitiveComponent* other, float restitution) override;

	std::string GetType() const override { return "Sphere"; }
};