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

	// 비인덱싱 메시를 위한 생성자 오버로딩
	UMesh(const TArray<FVertexPosColor4>& InVertices, D3D_PRIMITIVE_TOPOLOGY InPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		: Vertices(InVertices), PrimitiveType(InPrimitiveType)
	{
		NumVertices = Vertices.size();
		NumIndices = 0; // 인덱스가 없으므로 0
		Stride = sizeof(FVertexPosColor4);
		LocalBounds = ComputeLocalBounds(Vertices);
	}

	// 인덱싱 메시를 위한 생성자
	UMesh(const TArray<FVertexPosColor4>& InVertices, const TArray<uint32>& InIndices, D3D_PRIMITIVE_TOPOLOGY InPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		: Vertices(InVertices), Indices(InIndices), PrimitiveType(InPrimitiveType)
	{
		NumVertices = Vertices.size();
		NumIndices = Indices.size();
		Stride = sizeof(FVertexPosColor4);
		LocalBounds = ComputeLocalBounds(Vertices);
	}

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
		if (IndexBuffer) IndexBuffer->Release();
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return bIsInitialized; }

	const FBounds& GetLocalBounds() const { return LocalBounds; }
	const FBounds& GetRealBounds() const { return RealBounds; }	// 지금 안씀

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
	static FBounds CalculateAccurateWorldBounds(const UMesh* InMesh, const FMatrix& WorldTransform)
	{
		// 원본 정점 데이터의 '사본'을 만듭니다
		TArray<FVertexPosColor4> WorldVertices = InMesh->Vertices;

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

		return AccurateWorldBounds;
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
	FBounds RealBounds{};	// 지금 안씀
};
