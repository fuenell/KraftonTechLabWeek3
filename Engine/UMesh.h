// UMesh.h
#pragma once
#include "stdafx.h"

class FVertexSimple; // 전방 선언

class UMesh
{
public:
	ID3D11Buffer* VertexBuffer = nullptr;
	int NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = 0;

	// 생성자에서 초기화 리스트와 버텍스 버퍼를 생성
	UMesh(ID3D11Device* device, const std::vector<FVertexSimple>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}
};
