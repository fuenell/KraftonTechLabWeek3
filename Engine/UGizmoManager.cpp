#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoArrowComp.h"
#include "UGizmoGridComp.h"

UGizmoManager::UGizmoManager(UMeshManager* meshMgr) : meshManager(meshMgr)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = new UGizmoGridComp();
	gridPrimitive->Init(meshManager);

	// --- 2. 이동 기즈모(화살표) 생성 ---
	UGizmoArrowComp* arrowX = new UGizmoArrowComp();
	arrowX->Init(meshManager);
	UGizmoArrowComp* arrowY = new UGizmoArrowComp();
	arrowY->Init(meshManager);
	UGizmoArrowComp* arrowZ = new UGizmoArrowComp();
	arrowZ->Init(meshManager);

	arrowX->SetRotation({ 0.0f, 0.0f, 90.0f });
	arrowY->SetRotation({ -90.0f, 0.0f, 0.0f });

	transformGizmoPrimitives.push_back(arrowX);
	transformGizmoPrimitives.push_back(arrowY);
	transformGizmoPrimitives.push_back(arrowZ);
}

UGizmoManager::~UGizmoManager()
{
}

void UGizmoManager::SetTarget(UPrimitiveComponent* target)
{
	targetObject = target;
}

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
