#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "USceneComponent.h"
#include "Vector.h"

class UMeshManager; // 전방 선언


class UPrimitiveComponent : public USceneComponent
{
	DECLARE_UCLASS(UPrimitiveComponent, USceneComponent)

public:
	UPrimitiveComponent(FVector loc = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: USceneComponent(loc, rot, scl), Mesh(nullptr)
	{
	}

	virtual void Draw(URenderer& Renderer);
	virtual void UpdateConstantBuffer(URenderer& Renderer);
	virtual ~UPrimitiveComponent() {}

	// 별도의 초기화 메서드
	virtual bool Init(UMeshManager* MeshManager);

	bool CountOnInspector() override { return true; }

	UMesh* GetMesh() { return Mesh; }

	void SetColor(const FVector4& NewColor) { Color = NewColor; }
	FVector4 GetColor() const { return Color; }

	bool IsManageable() override { return true; }


	void SetBoundingBox(FBounds InBoundingBox) { BoundingBox = InBoundingBox; }

	FBounds GetBoundingBox() const { return BoundingBox; }

	// 실제 딱 맞는 Bounds를 계산하는 함수 (고비용)
	void UpdateAccurateBounds(const FMatrix& WorldTransform);

public:
	bool bIsSelected = false;

protected:
	UMesh* Mesh;
	FVector4 Color = { 1, 1, 1, 1 };

	FBounds BoundingBox;

	FMatrix LastAccurateBoundsTransform{};	// 기본 행렬로 초기화
};
