#pragma once
#include "UMesh.h"
#include "URenderer.h"
#include "UEngineSubsystem.h"

class UMeshManager : UEngineSubsystem
{
	DECLARE_UCLASS(UMeshManager, UEngineSubsystem)
public:
	UMeshManager();
	~UMeshManager();

	bool Initialize(URenderer* Renderer);
	UMesh* RetrieveMesh(FString MeshName);

private:
	UMesh* CreateMeshInternal(const TArray<FVertexPosColor>& Vertices, D3D_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UMesh* CreateIndexMeshInternal(const TArray<FVertexPosColor>& Vertices, const TArray<uint32>& InIndices, D3D_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

private:
	TMap<FString, UMesh*> Meshes;
};
