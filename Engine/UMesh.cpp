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
			// HR_CHECK 매크로나 별도 예외 처리를 사용하는 것이 좋습니다.
			throw std::runtime_error("Failed to create vertex buffer for mesh");
		}
	}

	bIsInitialized = true;
}