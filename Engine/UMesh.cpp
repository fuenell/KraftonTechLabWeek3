#include "stdafx.h"
#include "UMesh.h"

void UMesh::Initialize(ID3D11Device* device)
{
    NumVertices = static_cast<int>(vertices.size());

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(FVertexSimple) * NumVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&bd, &initData, &VertexBuffer);
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create vertex buffer for mesh");
    }

    else isInitialized = true;
}
