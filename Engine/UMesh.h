// UMesh.h
#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "Vector4.h"
#include "Matrix.h"

struct FVertexPosColor4; // 전방 선언

//struct FBounds
//{
//	FVector Min;
//	FVector Max;
//};

class UMesh : public UObject
{
	DECLARE_UCLASS(UMesh, UObject)
public:
	UMesh() {}

	// 인덱싱 메시를 위한 생성자
	UMesh(const TArray<FVertexPosColor4>& InVertices, const TArray<uint32>& InIndices, D3D_PRIMITIVE_TOPOLOGY InPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		: Vertices(InVertices), Indices(InIndices), PrimitiveType(InPrimitiveType)
	{
		NumVertices = Vertices.size();
		NumIndices = Indices.size();
		Stride = sizeof(FVertexPosColor4);
		LocalBounds = ComputeLocalBounds(Vertices);
		AccurateBounds = LocalBounds;
	}

	// 비인덱싱 메시를 위한 생성자 (생성자 위임 사용)
	UMesh(const TArray<FVertexPosColor4>& InVertices, D3D_PRIMITIVE_TOPOLOGY InPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		: UMesh(InVertices, {}, InPrimitiveType)
	{
	}

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
		if (IndexBuffer) IndexBuffer->Release();
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return bIsInitialized; }

	const FBounds& GetLocalBounds() const { return LocalBounds; }
	const FBounds& GetAccurateBounds() const { return AccurateBounds; }

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

	// 실제 딱 맞는 Bounds를 계산하는 함수
	void UpdateAccurateBounds(const FMatrix& WorldTransform)
	{
		// 이전에 이미 계산을 수행했고, 파라미터로 들어온 WorldTransform이 마지막으로 사용된 것과 같다면
		if (LastAccurateBoundsTransform == WorldTransform)
		{
			return;
		}

		LastAccurateBoundsTransform = WorldTransform;

		// 원본 정점 데이터의 '사본'을 만듭니다
		TArray<FVertexPosColor4> WorldVertices = Vertices;

		// 사본 배열의 모든 정점을 하나씩 순회하며 월드 좌표로 변환합니다.
		for (FVertexPosColor4& Vertex : WorldVertices)
		{
			// 로컬 좌표를 월드 좌표로 변환
			FVector NewPosition = FMatrix::MultiplyVectorRow(Vertex.GetPosition(), WorldTransform).ToVec3Homogenized();

			// 변환된 좌표로 갱신
			Vertex.SetPosition(NewPosition);
		}

		// 변환된 '모든' 월드 좌표 정점들을 기반으로 딱 맞는 새로운 AABB를 계산합니다.
		const FBounds& AccurateWorldBounds = UMesh::ComputeLocalBounds(WorldVertices);

		AccurateBounds = AccurateWorldBounds;
	}

public:
	// 공용 멤버 변수
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Vertex Buffer 관련 데이터
	ID3D11Buffer* VertexBuffer = nullptr;
	TArray<FVertexPosColor4> Vertices;
	int32 NumVertices = 0;
	UINT Stride = 0;

	// Index Buffer 관련 데이터
	ID3D11Buffer* IndexBuffer = nullptr;
	TArray<uint32> Indices;
	int32 NumIndices = 0;

private:
	bool bIsInitialized = false;

	FBounds LocalBounds{};
	FBounds AccurateBounds{};

	FMatrix LastAccurateBoundsTransform{};	// 기본 행렬
};
