#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"

class UPrimitiveComponent : public USceneComponent
{
public:
	virtual void UpdatePhysics(float t, bool bUsingGravity, float restitution) = 0;
	virtual void Draw(URenderer& renderer) = 0;
	virtual bool OnCollision(UPrimitiveComponent* other, float restitution) = 0;
	virtual void UpdateConstantBuffer(URenderer& renderer) = 0;
	virtual ~UPrimitiveComponent() {}

protected:
	UMesh* mesh;
};
