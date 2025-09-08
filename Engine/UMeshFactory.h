#pragma once
#include "UMesh.h"
#include "URenderer.h"
#include <vector>
#include "GizmoVertices.h"


class UMeshFactory
{
private:
	// 공통 로직을 처리하는 내부 함수
	template <size_t N>
	static UMesh* CreateMeshInternal(ID3D11Device* device, const FVertexPosColor(&vertices)[N], D3D_PRIMITIVE_TOPOLOGY primitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices, N);
		UMesh* mesh = new UMesh(device, convertedVertices, primitiveType);
		return mesh;
	}
public:
	static UMesh* CreateSphereMesh(URenderer& renderer)
	{
		return CreateMeshInternal(renderer.GetDevice(), sphere_vertices);
	}


	// Gizmo 메쉬 생성 함수들

	static UMesh* CreateGizmoArrowMesh(URenderer& renderer)
	{
		return CreateMeshInternal(renderer.GetDevice(), gizmo_arrow_vertices);
	}

	static UMesh* CreateGizmoGridMesh(URenderer& renderer)
	{
		FVertexPosColor::ChangeAxis(gizmo_grid_vertices, sizeof(gizmo_grid_vertices) / sizeof(FVertexPosColor), 1, 2);
		return CreateMeshInternal(renderer.GetDevice(), gizmo_grid_vertices, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
};