#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"


class UPrimitiveComponent : public USceneComponent
{
public:
	UPrimitiveComponent(UMesh* inMesh = nullptr, FVector pos = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(pos, rot, scl)
		, mesh(inMesh)
	{
	}

	bool bIsSelected = false;
	
	virtual void UpdatePhysics(float t, bool bUsingGravity, float restitution) = 0;
	virtual void Draw(URenderer& renderer) = 0;
	virtual bool OnCollision(UPrimitiveComponent* other, float restitution) = 0;
	virtual void UpdateConstantBuffer(URenderer& renderer) = 0;
	virtual ~UPrimitiveComponent() {}
	std::string GetType() const override { return "UPrimitiveComponent"; }

	UMesh& GetMesh() { return *mesh; }

protected:
	UMesh* mesh;
};
