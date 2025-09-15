#include "stdafx.h"
#include "UMesh.h"
#include "FVertexPosColor.h"

IMPLEMENT_UCLASS(UMesh, UObject)

void UMesh::Init(ID3D11Device* Device)
{
	if (bIsInitialized || !Device) return;

	// 1. Vertex Buffer 생성
	if (NumVertices > 0)
	{
		D3D11_BUFFER_DESC Vbd = {};
		Vbd.Usage = D3D11_USAGE_IMMUTABLE;
		Vbd.ByteWidth = sizeof(FVertexPosColor4) * NumVertices;
		Vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		Vbd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA Vsd = {};
		Vsd.pSysMem = Vertices.data();

		HRESULT Hr = Device->CreateBuffer(&Vbd, &Vsd, &VertexBuffer);
		if (FAILED(Hr))
		{
			throw std::runtime_error("Failed to create vertex buffer for mesh");
		}
	}

	// 2. Index Buffer 생성 (추가된 부분)
	// 인덱스가 있는 경우에만 생성합니다.
	if (NumIndices > 0)
	{
		D3D11_BUFFER_DESC IBD;
		IBD.Usage = D3D11_USAGE_IMMUTABLE;
		IBD.ByteWidth = sizeof(uint32) * NumIndices;
		IBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
		IBD.CPUAccessFlags = 0;
		IBD.MiscFlags = 0;
		IBD.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA ISD;
		ISD.pSysMem = Indices.data();
		HRESULT Hr = Device->CreateBuffer(&IBD, &ISD, &IndexBuffer);
		if (FAILED(Hr))
		{
			throw std::runtime_error("Failed to create index buffer for mesh");
		}
	}

	bIsInitialized = true;
}