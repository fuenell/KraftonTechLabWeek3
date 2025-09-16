#include "stdafx.h"
#include "UMeshManager.h"
#include "GizmoVertices.h"
#include "SphereVertices.h"
#include "PlaneVertices.h"
#include "CubeVertices.h"
#include "CapsuleVertices.h"
#include "USphereComp.h"
#include "UPlaneComp.h"
#include "UCubeComp.h"
#include "USpotLightComponent.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"
#include "UCapsuleComp.h"

IMPLEMENT_UCLASS(UMeshManager, UEngineSubsystem)
UMesh* UMeshManager::CreateMeshInternal(const TArray<FVertexPosColor>& Vertices, D3D_PRIMITIVE_TOPOLOGY PrimitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(Vertices.data(), Vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, PrimitiveType);
	return mesh;
}

UMesh* UMeshManager::CreateIndexMeshInternal(const TArray<FVertexPosColor>& Vertices, const TArray<uint32>& InIndices, D3D_PRIMITIVE_TOPOLOGY PrimitiveType)
{
	// vector의 데이터 포인터와 크기를 ConvertVertexData에 전달
	auto convertedVertices = FVertexPosColor4::ConvertVertexData(Vertices.data(), Vertices.size());
	UMesh* mesh = new UMesh(convertedVertices, InIndices, PrimitiveType);
	return mesh;
}

// 생성자
UMeshManager::UMeshManager()
{
	Meshes[UCapsuleComp::StaticClass()->GetUClassName()] = CreateIndexMeshInternal(CapsuleVertices, CapsuleIndices);
	Meshes[USphereComp::StaticClass()->GetUClassName()] = CreateMeshInternal(SphereVertices);
	Meshes[UPlaneComp::StaticClass()->GetUClassName()] = CreateMeshInternal(PlaneVertices);
	Meshes[UCubeComp::StaticClass()->GetUClassName()] = CreateIndexMeshInternal(CubeVertices, CubeIndices);
	Meshes[USpotLightComponent::StaticClass()->GetUClassName()] = CreateMeshInternal(PlaneVertices);

	Meshes[UGizmoArrowComp::StaticClass()->GetUClassName()] = CreateMeshInternal(GizmoArrowVertices);
	Meshes[UGizmoRotationHandleComp::StaticClass()->GetUClassName()] = CreateMeshInternal(GridGenerator::CreateRotationHandleVertices());
	Meshes[UGizmoScaleHandleComp::StaticClass()->GetUClassName()] = CreateMeshInternal(GizmoScaleHandleVertices);
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
