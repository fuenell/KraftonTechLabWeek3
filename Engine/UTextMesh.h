#pragma once

#include "stdafx.h"
#include "FVertexPosUV.h"
#include "Vector4.h"

class UTextMesh : public UObject
{
	DECLARE_UCLASS(UTextMesh, UObject)
private:
	bool isInitialized = false;
public:
	ID3D11Buffer* VertexBuffer = nullptr;
	TArray<FVertexPosUV4> Vertices;
	int32 NumVertices = 0;
	D3D_PRIMITIVE_TOPOLOGY PrimitiveType;
	UINT Stride = 0;

	UTextMesh();
	UTextMesh(const TArray<FVertexPosUV4>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	~UTextMesh()
	{
		if (VertexBuffer) VertexBuffer->Release();
	}

	void Init(ID3D11Device* device);
	void UpdateMesh();

	bool IsInitialized() const { return isInitialized; }
};