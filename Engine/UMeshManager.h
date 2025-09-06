#pragma once

#include "UMesh.h"
#include "URenderer.h"

class UMeshManager
{
private:
    std::unordered_map<std::string, UMesh*> meshes;

    // 공통 로직을 처리하는 내부 템플릿 함수
    template <size_t N>
    UMesh* CreateMeshInternal(const FVertexPosColor(&vertices)[N],
        D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

public:
    UMeshManager();
    ~UMeshManager();

    bool Initialize(URenderer* renderer);
    UMesh* RetrieveMesh(std::string meshName);

    // 기본 메쉬 생성 함수들
    UMesh* CreateSphereMesh();
    UMesh* CreateGizmoArrowMesh();
    UMesh* CreateGizmoGridMesh();
};
