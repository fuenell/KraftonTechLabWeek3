// UMesh.h
#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "UObject.h"
#include "Vector4.h"

struct FVertexPosColor4; // 전방 선언

class UMesh : public UObject
{
private:
	bool isInitialized = false;
public:
	ID3D11Buffer* VertexBuffer = nullptr;
	std::vector<FVertexPosColor4> Vertices;
	int32 NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = 0;

	// 생성자에서 초기화 리스트와 버텍스 버퍼를 생성
	UMesh(const std::vector<FVertexPosColor4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}

	void Init(ID3D11Device* device);

	bool IsInitialized() const { return isInitialized; }

};
