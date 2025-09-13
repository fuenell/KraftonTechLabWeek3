// UMesh.h
#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "Vector4.h"

struct FVertexPosColor4; // 전방 선언


class UMesh : public UObject
{
	DECLARE_UCLASS(UMesh, UObject)
private:
	bool isInitialized = false;
public:
	ID3D11Buffer* VertexBuffer = nullptr;
	TArray<FVertexPosColor4> Vertices;
	int32 NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = 0;
	FBounds LocalBounds;

	UMesh();
	// 생성자에서 초기화 리스트와 버텍스 버퍼를 생성
	UMesh(const TArray<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return isInitialized; }

	const FBounds& GetLocalBounds() const { return LocalBounds; }

	// 여기서 최소값과 최대값이 결정된 두개의 점 구조체가 완성됨
	static FBounds ComputeLocalBounds(const TArray<FVertexPosColor4>& verts)
	{
		FBounds b;
		b.Min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		b.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
		for (const auto& v : verts)
		{
			FVector p(v.x, v.y, v.z);
			b.Min.X = (p.X < b.Min.X) ? p.X : b.Min.X;
			b.Min.Y = (p.Y < b.Min.Y) ? p.Y : b.Min.Y;
			b.Min.Z = (p.Z < b.Min.Z) ? p.Z : b.Min.Z;

			b.Max.X = (p.X > b.Max.X) ? p.X : b.Max.X;
			b.Max.Y = (p.Y > b.Max.Y) ? p.Y : b.Max.Y;
			b.Max.Z = (p.Z > b.Max.Z) ? p.Z : b.Max.Z;
		}
		return b;
	}
};
