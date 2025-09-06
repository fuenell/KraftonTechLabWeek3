#pragma once
#include "UMesh.h"
#include "URenderer.h"
#include <vector>

class UMeshFactory
{
public:
	// Sphere Mesh 생성
	static UMesh* CreateSphereMesh()
	{
		// 기존 데이터를 새 형식으로 변환
		int numVertices = sizeof(sphere_vertices) / sizeof(FVertexSimpleOld);
		auto convertedVertices = FVertexSimple::ConvertVertexData(
			sphere_vertices,
			numVertices
		);

		UMesh* mesh = new UMesh(convertedVertices);
		//mesh->Initialize(renderer.GetDevice(), convertedVertices); 사용할 때만 mesh를 init하도록 변경
		return mesh;
	}
};