#pragma once
#include "stdafx.h"
#include "UTextMesh.h"
#include "USceneComponent.h"
#include "Vector.h"

// class UMeshManager

struct FVertexPosUV4;

class UTextComponent : public USceneComponent
{
	DECLARE_UCLASS(UTextComponent, USceneComponent)
protected:
	UTextMesh* TMesh;
	FVector4 Color = { 1, 1, 1, 1 };
	PrimitiveType Type;
public:
	UTextComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), TMesh(nullptr)
	{
	}

	bool bIsSelected = false;

	virtual void Draw(URenderer& renderer);
	virtual void UpdateConstantBuffer(URenderer& renderer);
	virtual ~UTextComponent() {}

	// 별도의 초기화 메서드
	//virtual bool Init(UMeshManager* meshManager);

	bool CountOnInspector() override { return true; }

	PrimitiveType GetType()const { return Type; }

	UTextMesh* GetMesh() { return TMesh; }
};