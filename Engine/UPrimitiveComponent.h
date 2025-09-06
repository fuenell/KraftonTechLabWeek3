#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언


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
	UPrimitiveComponent(FVector loc = {0,0,0}, FVector rot = {0,0,0}, FVector scl = {1,1,1})
		: USceneComponent(loc, rot, scl), mesh(nullptr) {}
	virtual ~UPrimitiveComponent() {}
	
	// 별도의 초기화 메서드
	virtual bool Init(UMeshManager* meshManager);
	std::string GetType() const override { return "UPrimitiveComponent"; }
	bool CountOnInspector() override { return true; } 

	UMesh& GetMesh() { return *mesh; }

protected:
	UMesh* mesh;
};
