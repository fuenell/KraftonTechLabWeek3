#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언

class UGizmoComponent : public USceneComponent
{
public:
	UGizmoComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr)
	{
	}

	bool bIsSelected = false;

	virtual void Draw(URenderer& renderer) = 0;
	virtual void UpdateConstantBuffer(URenderer& renderer) = 0;
	virtual ~UGizmoComponent() {}

	// 별도의 초기화 메서드
	bool Init(UMeshManager* meshManager);
	std::string GetType() const override { return "UGizmoComponent"; }
	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return mesh; }

protected:
	UMesh* mesh;
};
