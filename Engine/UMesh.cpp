#include "stdafx.h"
#include "UMesh.h"
#include "FVertexSimple.h"

UMesh::UMesh(ID3D11Device* device, const std::vector<FVertexSimple>& vertices, D3D_PRIMITIVE_TOPOLOGY primitiveType)
	: PrimitiveType(primitiveType), NumVertices(vertices.size()), Stride(sizeof(FVertexSimple))
{
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(FVertexSimple) * NumVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices.data();

	HRESULT hr = device->CreateBuffer(&vbd, &vertexData, &VertexBuffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("Failed to create vertex buffer for mesh");
	}
}