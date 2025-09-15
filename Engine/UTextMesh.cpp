#include "stdafx.h"
#include "UTextMesh.h"


void UTextMesh::Init(ID3D11Device * device)
{
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(FVertexPosUV) * 3; // 실제로는 문자열이 없지만, Draw 시에 아무것도 그리지 않을 예정
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};

	std::vector<FVertexPosUV> vertices;

	vertices.push_back({ -0.5f,  0.5f, 0.0f, 0, 0 }); // 좌상
	vertices.push_back({  0.5f, -0.5f, 0.0f, 0, 1 }); // 우상
	vertices.push_back({ -0.5f, -0.5f, 0.0f, 1, 1 }); // 우하

	vertexData.pSysMem = Vertices.data();

	HRESULT hr = device->CreateBuffer(&vbd, &vertexData, &VertexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create vertex buffer for text mesh");
	}

	isInitialized = true;
}

void UTextMesh::UpdateMesh()
{

}