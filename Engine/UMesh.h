// UMesh.h
#pragma once
#include "FVertexSimple.h"
#include "stdafx.h"

class UMesh
{
public:
    ID3D11Buffer* VertexBuffer = nullptr;
    int NumVertices = 0;

    void Initialize(ID3D11Device* device, const std::vector<FVertexSimple>& vertices);

    ~UMesh()
    {
        if (VertexBuffer) VertexBuffer->Release();
    }
};
