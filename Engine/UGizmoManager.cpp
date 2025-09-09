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
	delete gridPrimitive;
	for (auto gizmo : transformGizmoPrimitives)
	{
		delete gizmo;
	}
	transformGizmoPrimitives.clear();
}

bool UGizmoManager::Initialize(UMeshManager* meshManager)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = new UGizmoGridComp();

	UGizmoArrowComp* arrowX = new UGizmoArrowComp();
	arrowX->Axis = EAxis::X;
	UGizmoArrowComp* arrowY = new UGizmoArrowComp();
	arrowY->Axis = EAxis::Y;
	UGizmoArrowComp* arrowZ = new UGizmoArrowComp();
	arrowZ->Axis = EAxis::Z;

	if (!gridPrimitive->Init(meshManager) || !arrowX->Init(meshManager) || !arrowY->Init(meshManager) || !arrowZ->Init(meshManager))
	{
		delete gridPrimitive;
		delete arrowX;
		delete arrowY;
		delete arrowZ;
		return false;
	}

	arrowX->SetColor({ 1, 0, 0, 1 });
	arrowY->SetColor({ 0, 1, 0, 1 });
	arrowZ->SetColor({ 0, 0, 1, 1 });

	// --- 2. 이동 기즈모(화살표) 생성 ---
	bool gridArrowSuccess = true;

	arrowX->SetRotation({ 0.0f, 0.0f, -90.0f });
	arrowY->SetRotation({ 0.0f, 90.0f, 0.0f });
	arrowZ->SetRotation({ 90.0f, 0.0f, 0.0f });

	transformGizmoPrimitives.push_back(arrowX);
	transformGizmoPrimitives.push_back(arrowZ);
	transformGizmoPrimitives.push_back(arrowY);

	return true;
}

void UGizmoManager::SetTarget(UPrimitiveComponent* target)
{
	targetObject = target;
}

TArray<UGizmoComponent*>& UGizmoManager::GetRaycastableGizmos()
{
	if (targetObject == nullptr)
	{
		static TArray<UGizmoComponent*> emptyArray; // lives for the whole program
		return emptyArray;
	}
	return transformGizmoPrimitives;
}

bool UGizmoManager::IsRaycastHit(URaycastManager* rayCastManager, UGizmoComponent& out)
{
	// for (UMesh* gizmoMesh : transformGizmoPrimitives)
	// {
	// 	if (FMath::IntersectRayMesh(ray, gizmoMesh, m_ActiveGizmo->GetWorldTransform(), hitResult))
	// 	{
	// 		// Gizmo에 특화된 결과 처리
	// 		return true;
	// 	}
	// }
	return false;
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

//레이와 이동 평면의 3D 교차점 찾기
FVector UGizmoManager::FindCirclePlaneIntersection(const FRay& ray, const FPlane& plane)
{
	float denominator = ray.Direction.Dot(plane.Normal);

	// 레이가 평면과 평행에 가까우면 계산 오류 방지
	if (abs(denominator) < 0.0001f) return { 0,0,0 };

	float t = (plane.PointOnPlane - ray.Origin).Dot(plane.Normal) / denominator;
	FVector intersectionPoint = ray.Origin + ray.Direction * t;

	return intersectionPoint;
}

void UGizmoManager::BeginDrag(const FRay& mouseRay, EAxis axis)
{
	isDragging = true;
	selectedAxis = axis;
	dragStartLocation = targetObject->GetPosition();

	// --- 이동 평면 생성 ---
	movementPlane.PointOnPlane = dragStartLocation;

	FVector axisDir = GetAxisVector(selectedAxis);
	FVector camToObjectDir = (dragStartLocation - mouseRay.Origin).GetNormalized();

	// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
	// 다시 외적하여 평면의 법선 벡터를 계산
	FVector tempVec = axisDir.Cross(camToObjectDir);
	movementPlane.Normal = axisDir.Cross(tempVec).GetNormalized();

	// 선택한 곳을 offset 으로 저장해서 드래그 시 중심점으로 이동하지 않게 하기
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	float projectedLength = startToIntersectionVec.Dot(axisDir);
	FVector newPosition = dragStartLocation + axisDir * projectedLength;

	dragOffset = dragStartLocation - newPosition;
}

void UGizmoManager::UpdateDrag(const FRay& mouseRay)
{
	if (!isDragging) return;

	// --- 1. 마우스 레이와 이동 평면의 3D 교차점 찾기 ---
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);

	// --- 2. 3D 교차점을 이동 축으로 투영하기 ---
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	FVector axisDir = GetAxisVector(selectedAxis);

	float projectedLength = startToIntersectionVec.Dot(axisDir);
	FVector newPosition = dragStartLocation + axisDir * projectedLength;

	// --- 3. Target 액터 위치 업데이트 ---
	targetObject->SetPosition(newPosition + dragOffset);
}

void UGizmoManager::EndDrag()
{
	isDragging = false;
	selectedAxis = EAxis::None;
}

FVector UGizmoManager::GetAxisVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 1,0,0 };
	case EAxis::Y:
		return { 0,1,0 };
	case EAxis::Z:
		return { 0,0,1 };
	default:
		return { 0,0,0 };
	}
}
