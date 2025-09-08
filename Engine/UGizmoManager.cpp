#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UGizmoArrowComp.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"

UGizmoManager::UGizmoManager()
{
}

UGizmoManager::~UGizmoManager()
{
}

bool UGizmoManager::Initialize(UMeshManager* meshManager)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = new UGizmoGridComp();

	UGizmoArrowComp* arrowX = new UGizmoArrowComp();
	UGizmoArrowComp* arrowY = new UGizmoArrowComp();
	UGizmoArrowComp* arrowZ = new UGizmoArrowComp();

	if (!gridPrimitive->Init(meshManager) || !arrowX->Init(meshManager) || !arrowY->Init(meshManager) || !arrowZ->Init(meshManager))
	{
		delete gridPrimitive;
		delete arrowX;
		delete arrowY;
		delete arrowZ;
		return false;
	}

	// --- 2. 이동 기즈모(화살표) 생성 ---
	bool gridArrowSuccess = true;

	arrowX->SetRotation({ 0.0f, 0.0f, 90.0f });
	arrowY->SetRotation({ -90.0f, 0.0f, 0.0f });
	arrowZ->SetRotation({ 0.0f, 90.0f, 0.0f });

	transformGizmoPrimitives.push_back(arrowX);
	transformGizmoPrimitives.push_back(arrowZ);
	transformGizmoPrimitives.push_back(arrowY);

	return true;
}

void UGizmoManager::SetTarget(UPrimitiveComponent* target)
{
	targetObject = target;
}

//bool UGizmoManager::Raycast(URaycastManager* rayCastManager, UGizmoComponent& out)
//{
//	//for (UMesh* gizmoMesh : transformGizmoPrimitives)
//	//{
//	//	if (FMath::IntersectRayMesh(ray, gizmoMesh, m_ActiveGizmo->GetWorldTransform(), hitResult))
//	//	{
//	//		// Gizmo에 특화된 결과 처리
//	//		return true;
//	//	}
//	//}
//	return false;
//}

void UGizmoManager::Draw(URenderer& renderer)
{
	// --- 파트 1: 타겟 유무와 상관없이 항상 그리는 요소 ---
	if (gridPrimitive)
	{
		gridPrimitive->Draw(renderer);
	}

	// --- 파트 2: 타겟이 있을 때만 그리는 요소 ---
	if (targetObject != nullptr)
	{
		// 타겟의 현재 위치를 가져옵니다.
		FVector targetPosition = targetObject->GetPosition();

		// 모든 트랜스폼 기즈모 파츠(화살표들)를 순회하며 위치를 업데이트하고 그립니다.
		for (auto gizmoPart : transformGizmoPrimitives)
		{
			if (gizmoPart)
			{
				gizmoPart->SetPosition(targetPosition);
				gizmoPart->Draw(renderer);
			}
		}
	}
}
