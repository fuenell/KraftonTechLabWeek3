#pragma once

#include "UMesh.h"
#include "URenderer.h"
#include "UEngineSubsystem.h"

class UMeshManager : UEngineSubsystem
{
private:
	std::unordered_map<std::string, UMesh*> meshes;

	UMesh* CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
		D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* renderer);
	UMesh* RetrieveMesh(std::string meshName);

	// 기본 메쉬 생성 함수들
	UMesh* CreateSphereMesh();
	UMesh* CreatePlaneMesh();
	UMesh* CreateGizmoArrowMesh();
	UMesh* CreateGizmoGridMesh();
};
