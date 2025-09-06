// UMesh.h
#pragma once
#include "FVertexSimple.h"
#include "stdafx.h"

class UMesh
{
public:
    ID3D11Buffer* VertexBuffer = nullptr;
    std::vector<FVertexSimple> vertices;
    int NumVertices = 0;
    bool isInitialized = false;

    void Initialize(ID3D11Device* device);

    // 생성자에서는 버텍스 데이터만 저장
    UMesh(const std::vector<FVertexSimple>& verts) : vertices(verts) {
        NumVertices = static_cast<int>(verts.size());
    }

    ~UMesh()
    {
        if (VertexBuffer) VertexBuffer->Release();
    }
};
