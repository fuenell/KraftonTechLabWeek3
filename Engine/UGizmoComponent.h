#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언

class UGizmoComponent : public USceneComponent
{
public:
	FVector OriginRotation{ 0,0,0 };

	UGizmoComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), mesh(nullptr)
	{
	}

	bool bIsSelected = false;

	virtual ~UGizmoComponent()
	{
	}

	bool Init(UMeshManager* meshManager);
	std::string GetType() const override { return "UGizmoComponent"; }
	bool CountOnInspector() override { return true; }

	FMatrix GetWorldTransform() override;

	virtual void Draw(URenderer& renderer);
	virtual void UpdateConstantBuffer(URenderer& renderer);

	UMesh* GetMesh() { return mesh; }

	void SetOriginRotation(FVector originRotation)
	{
		OriginRotation = originRotation;
	}

	void SetColor(const FVector4& newColor) { Color = newColor; }
	FVector4 GetColor() const { return Color; }

protected:
	UMesh* mesh;
	FVector4 Color = { 1, 1, 1, 1 };
};
