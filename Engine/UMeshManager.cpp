#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"
#include "SphereVertices.h"
#include "PlaneVertices.h"
#include "CubeVertices.h"
#include "CapsuleVertices.h"

IMPLEMENT_UCLASS(UMeshManager, UEngineSubsystem)
UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& Vertices, D3D_PRIMITIVE_TOPOLOGY PrimitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(Vertices.data(), Vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, PrimitiveType);
	return mesh;
}

// 생성자
UMeshManager::UMeshManager()
{
	Meshes["Capsule"] = CreateMeshInternal(CapsuleVertices);
	Meshes["Sphere"] = CreateMeshInternal(SphereVertices);
	Meshes["Plane"] = CreateMeshInternal(PlaneVertices);
	Meshes["Cube"] = CreateMeshInternal(CubeVertices);
	Meshes["SpotLight"] = CreateMeshInternal(PlaneVertices);
	//meshes["GizmoGrid"] = CreateMeshInternal(GridGenerator::CreateGridVertices(1, 100), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Meshes["GizmoArrow"] = CreateMeshInternal(GizmoArrowVertices);
	Meshes["GizmoRotationHandle"] = CreateMeshInternal(GridGenerator::CreateRotationHandleVertices());
	Meshes["GizmoScaleHandle"] = CreateMeshInternal(GizmoScaleHandleVertices);
}

// 소멸자 (메모리 해제)
UMeshManager::~UMeshManager()
{
	for (auto& pair : Meshes)
	{
		delete pair.second;
	}
	Meshes.clear();
}

bool UMeshManager::Initialize(URenderer* Renderer)
{
	if (!Renderer) return false;

	try
	{
		for (const auto& var : Meshes)
		{
			var.second->Init(Renderer->GetDevice());
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

UMesh* UMeshManager::RetrieveMesh(FString MeshName)
{
	auto itr = Meshes.find(MeshName);
	if (itr == Meshes.end()) return nullptr;
	return itr->second;
}
