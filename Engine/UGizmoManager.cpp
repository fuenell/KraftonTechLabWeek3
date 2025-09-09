#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UGizmoArrowComp.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UGizmoScaleHandleComp.h"

UGizmoManager::UGizmoManager()
{
}

UGizmoManager::~UGizmoManager()
{
	delete gridPrimitive;
	for (auto gizmo : locationGizmos)
	{
		delete gizmo;
	}
	locationGizmos.clear();
	for (auto gizmo : rotationGizmos)
	{
		delete gizmo;
	}
	rotationGizmos.clear();
	for (auto gizmo : scaleGizmos)
	{
		delete gizmo;
	}
	scaleGizmos.clear();
}

bool UGizmoManager::Initialize(UMeshManager* meshManager)
{
	// --- 1. 그리드 생성 ---
	// 그리드는 항상 원점에 고정
	gridPrimitive = new UGizmoGridComp();

	// =================================================

	UGizmoArrowComp* arrowX = new UGizmoArrowComp();
	arrowX->Axis = EAxis::X;
	UGizmoArrowComp* arrowY = new UGizmoArrowComp();
	arrowY->Axis = EAxis::Y;
	UGizmoArrowComp* arrowZ = new UGizmoArrowComp();
	arrowZ->Axis = EAxis::Z;

	arrowX->SetColor({ 1, 0, 0, 1 });
	arrowY->SetColor({ 0, 1, 0, 1 });
	arrowZ->SetColor({ 0, 0, 1, 1 });

	arrowX->SetRotation({ 0.0f, 0.0f, -90.0f });
	arrowY->SetRotation({ 0.0f, 90.0f, 0.0f });
	arrowZ->SetRotation({ 90.0f, 0.0f, 0.0f });

	locationGizmos.push_back(arrowX);
	locationGizmos.push_back(arrowZ);
	locationGizmos.push_back(arrowY);

	// =================================================

	UGizmoScaleHandleComp* scaleX = new UGizmoScaleHandleComp();
	scaleX->Axis = EAxis::X;
	UGizmoScaleHandleComp* scaleY = new UGizmoScaleHandleComp();
	scaleY->Axis = EAxis::Y;
	UGizmoScaleHandleComp* scaleZ = new UGizmoScaleHandleComp();
	scaleZ->Axis = EAxis::Z;

	scaleX->SetColor({ 1, 0, 0, 1 });
	scaleY->SetColor({ 0, 1, 0, 1 });
	scaleZ->SetColor({ 0, 0, 1, 1 });

	scaleX->SetRotation({ 0.0f, 0.0f, -90.0f });
	scaleY->SetRotation({ 0.0f, 90.0f, 0.0f });
	scaleZ->SetRotation({ 90.0f, 0.0f, 0.0f });

	scaleGizmos.push_back(scaleX);
	scaleGizmos.push_back(scaleZ);
	scaleGizmos.push_back(scaleY);

	if (!gridPrimitive->Init(meshManager) || !arrowX->Init(meshManager) || !arrowY->Init(meshManager) || !arrowZ->Init(meshManager)
		|| !scaleX->Init(meshManager) || !scaleY->Init(meshManager) || !scaleZ->Init(meshManager))
	{
		delete gridPrimitive;

		delete arrowX;
		delete arrowY;
		delete arrowZ;

		delete scaleX;
		delete scaleY;
		delete scaleZ;

		return false;
	}

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

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<UGizmoComponent*>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}
	return *currentGizmos;
}

void UGizmoManager::Draw(URenderer& renderer)
{
	// --- 파트 1: 타겟 유무와 상관없이 항상 그리는 요소 ---
	if (gridPrimitive)
	{
		gridPrimitive->Draw(renderer);
	}

	// --- 파트 2: 타겟이 있을 때만 그리는 요소 ---

	if (targetObject == nullptr) return;

	// 타겟의 위치를 가져옵니다.
	FVector targetPosition = targetObject->GetPosition();

	// 현재 모드에 따라 올바른 기즈모를 그립니다.
	TArray<UGizmoComponent*>* currentGizmos = nullptr;

	switch (translationType)
	{
	case ETranslationType::Location:
		currentGizmos = &locationGizmos;
		break;
	case ETranslationType::Rotation:
		currentGizmos = &rotationGizmos;
		break;
	case ETranslationType::Scale:
		currentGizmos = &scaleGizmos;
		break;
	}

	if (currentGizmos)
	{
		for (auto gizmoPart : *currentGizmos)
		{
			if (gizmoPart)
			{
				gizmoPart->SetPosition(targetPosition);
				gizmoPart->Draw(renderer);
			}
		}
	}
}

void UGizmoManager::NextTranslation()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging");
		return;
	}

	switch (translationType)
	{
	case ETranslationType::Location:
		translationType = ETranslationType::Rotation;
		UE_LOG("Rotation");
		break;
	case ETranslationType::Rotation:
		translationType = ETranslationType::Scale;
		UE_LOG("Scale");
		break;
	case ETranslationType::Scale:
		translationType = ETranslationType::Location;
		UE_LOG("Location");
		break;
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
