#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"

UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& vertices,
	D3D_PRIMITIVE_TOPOLOGY primitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(vertices.data(), vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, primitiveType);
	return mesh;
}

// 생성자
UMeshManager::UMeshManager()
{
	meshes["Sphere"] = CreateSphereMesh();
	meshes["GizmoArrow"] = CreateGizmoArrowMesh();
	meshes["GizmoGrid"] = CreateGizmoGridMesh();
}

// 소멸자 (메모리 해제)
UMeshManager::~UMeshManager()
{
	for (auto& pair : meshes)
	{
		delete pair.second;
	}
	meshes.clear();
}

bool UMeshManager::Initialize(URenderer* renderer)
{
	if (!renderer) return false;

	try
	{
		for (const auto& var : meshes)
		{
			var.second->Init(renderer->GetDevice());
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "UMeshManager::Initialize failed: " << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "UMeshManager::Initialize failed: unknown exception" << std::endl;
		return false;
	}

	return true;
}

UMesh* UMeshManager::RetrieveMesh(std::string meshName)
{
	auto itr = meshes.find(meshName);
	if (itr == meshes.end()) return nullptr;
	return itr->second;
}

UMesh* UMeshManager::CreateSphereMesh()
{
	return CreateMeshInternal(sphere_vertices);
}

UMesh* UMeshManager::CreateGizmoArrowMesh()
{
	return CreateMeshInternal(gizmo_arrow_vertices);
}

UMesh* UMeshManager::CreateGizmoGridMesh()
{
	return CreateMeshInternal(GridGenerator::CreateGridVertices(1, 20), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}
