#pragma once
#include "stdafx.h"
#include "UMesh.h"

class UPrimitive
{
public:
	virtual void UpdatePhysics(float t, bool bUsingGravity, float restitution) = 0;
	virtual void Draw(URenderer& renderer) = 0;
	virtual bool OnCollision(UPrimitive* other, float restitution) = 0;
	virtual void UpdateConstantBuffer(URenderer& renderer) = 0;
	virtual ~UPrimitive() {}

protected:
	UMesh* mesh;
};