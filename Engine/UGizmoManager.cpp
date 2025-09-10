#include "stdafx.h"
#include "UGizmoManager.h"
#include "UMeshManager.h"
#include "UApplication.h"
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "UGizmoGridComp.h"
#include "URaycastManager.h"
#include "UGizmoArrowComp.h"
#include "UGizmoRotationHandleComp.h"
#include "UGizmoScaleHandleComp.h"
#include "UObject.h"

IMPLEMENT_UCLASS(UGizmoManager, UEngineSubsystem)
UGizmoManager::UGizmoManager()
{
}

UGizmoManager::~UGizmoManager()
{
	gridPrimitive = nullptr;

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

	arrowX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	arrowY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	arrowZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	locationGizmos.push_back(arrowX);
	locationGizmos.push_back(arrowZ);
	locationGizmos.push_back(arrowY);

	// =================================================

	UGizmoRotationHandleComp* rotationX = new UGizmoRotationHandleComp();
	rotationX->Axis = EAxis::X;
	UGizmoRotationHandleComp* rotationY = new UGizmoRotationHandleComp();
	rotationY->Axis = EAxis::Y;
	UGizmoRotationHandleComp* rotationZ = new UGizmoRotationHandleComp();
	rotationZ->Axis = EAxis::Z;

	rotationX->SetColor({ 1, 0, 0, 1 });
	rotationY->SetColor({ 0, 1, 0, 1 });
	rotationZ->SetColor({ 0, 0, 1, 1 });

	rotationX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	rotationY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	rotationZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	rotationGizmos.push_back(rotationX);
	rotationGizmos.push_back(rotationZ);
	rotationGizmos.push_back(rotationY);

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

	scaleX->SetOriginRotation({ 0.0f, 0.0f, 90.0f });
	scaleY->SetOriginRotation({ 0.0f, 0.0f, 0.0f });
	scaleZ->SetOriginRotation({ -90.0f, 0.0f, 0.0f });

	scaleGizmos.push_back(scaleX);
	scaleGizmos.push_back(scaleZ);
	scaleGizmos.push_back(scaleY);

	// =================================================

	if (!gridPrimitive->Init(meshManager) || !arrowX->Init(meshManager) || !arrowY->Init(meshManager) || !arrowZ->Init(meshManager)
		|| !rotationX->Init(meshManager) || !rotationY->Init(meshManager) || !rotationZ->Init(meshManager)
		|| !scaleX->Init(meshManager) || !scaleY->Init(meshManager) || !scaleZ->Init(meshManager))
	{
		delete gridPrimitive;

		delete arrowX;
		delete arrowY;
		delete arrowZ;

		delete rotationX;
		delete rotationY;
		delete rotationZ;

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

void UGizmoManager::ChangeGizmoSpace()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Space");
		return;
	}

	isWorldSpace = !isWorldSpace;
}

void UGizmoManager::NextTranslation()
{
	if (isDragging)
	{
		UE_LOG("Now Dragging Can't Change Translation");
		return;
	}

	switch (translationType)
	{
	case ETranslationType::Location:
		translationType = ETranslationType::Rotation;
		break;
	case ETranslationType::Rotation:
		translationType = ETranslationType::Scale;
		break;
	case ETranslationType::Scale:
		translationType = ETranslationType::Location;
		break;
	}
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

void UGizmoManager::Update(float deltaTime)
{
	if (gridPrimitive)
	{
		gridPrimitive->Update(deltaTime);
	}
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
				// Todo: 이동을 update에서 처리해야 하나??
				gizmoPart->SetPosition(targetObject->GetPosition());

				// Scale 은 항상 local
				if (translationType != ETranslationType::Scale && isWorldSpace)
				{
					gizmoPart->ResetQuaternion();
				}
				else
				{
					gizmoPart->SetQuaternion(targetObject->RelativeQuaternion);
				}
				gizmoPart->DrawOnTop(renderer);
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

void UGizmoManager::BeginDrag(const FRay& mouseRay, EAxis axis, FVector impactPoint)
{
	isDragging = true;
	selectedAxis = axis;

	dragStartLocation = targetObject->GetPosition();
	dragStartScale = targetObject->GetScale();
	dragStartQuaternion = targetObject->GetQuaternion();

	// 이동 평면 생성
	movementPlane.PointOnPlane = dragStartLocation;

	// 사용자가 선택한 이동 축 벡터 (예: X축이면 (1,0,0))
	FVector axisDir = GetAxisVector(selectedAxis);

	// 카메라 위치에서 객체를 향하는 벡터 (시선 벡터)
	FVector camToObjectDir = (dragStartLocation - mouseRay.Origin).Normalized();

	if (translationType == ETranslationType::Rotation)
	{
		// 1. 회전이 일어날 평면을 정의 (법선: 회전축, 기준점: 객체 중심)
		movementPlane.Normal = axisDir;
		movementPlane.PointOnPlane = dragStartLocation;

		// 2. 평면과 마우스 레이의 교차점을 드래그 시작점으로 저장
		dragRotationStartPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);

		// 3. 드래그 방향(회전 접선 방향)을 계산하고 정규화
		FVector centerToStartVec = dragRotationStartPoint - dragStartLocation;
		dragRotationStartVector = axisDir.Cross(centerToStartVec).Normalized();

		return;
	}
	else if (translationType == ETranslationType::Location)
	{
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		}

		// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
		// 다시 외적하여 평면의 법선 벡터를 계산
		FVector tempVec = axisDir.Cross(camToObjectDir);
		movementPlane.Normal = axisDir.Cross(tempVec).Normalized();
	}
	else if (translationType == ETranslationType::Scale)
	{
		// Scale 은 항상 로컬 스페이스 모드
		axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);

		// 이동 축과 시선 벡터에 동시에 수직인 벡터를 찾고,
		// 다시 외적하여 평면의 법선 벡터를 계산
		FVector tempVec = axisDir.Cross(camToObjectDir);
		movementPlane.Normal = axisDir.Cross(tempVec).Normalized();
	}

	// 선택한 곳을 offset 으로 저장해서 드래그 시 중심점으로 이동하지 않게 하기
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	float projectedLength = startToIntersectionVec.Dot(axisDir);

	projectedLengthOffset = projectedLength;

	UE_LOG("%f", projectedLengthOffset);
}

void UGizmoManager::UpdateDrag(const FRay& mouseRay)
{
	if (!isDragging) return;

	// --- 1. 마우스 레이와 이동 평면의 3D 교차점 찾기 ---
	FVector intersectionPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);
	FVector startToIntersectionVec = intersectionPoint - dragStartLocation;
	FVector axisDir = GetAxisVector(selectedAxis);

	if (translationType == ETranslationType::Location)
	{
		// --- [위치] 업데이트 로직 ---
		// 로컬 스페이스 모드일 경우, 축 벡터를 오브젝트의 회전만큼 회전시킵니다.
		if (!isWorldSpace)
		{
			axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		}

		float projectedLength = startToIntersectionVec.Dot(axisDir) - projectedLengthOffset;
		FVector newPosition = dragStartLocation + axisDir * projectedLength;

		targetObject->SetPosition(newPosition);
	}
	else if (translationType == ETranslationType::Scale)
	{
		axisDir = targetObject->GetQuaternion().RotateInverse(axisDir);
		float projectedLength = startToIntersectionVec.Dot(axisDir) - projectedLengthOffset;
		FVector newScale = dragStartScale + GetAxisVector(selectedAxis) * projectedLength;

		const float minimumScale = 0.1f;

		newScale.X = max(newScale.X, minimumScale);
		newScale.Y = max(newScale.Y, minimumScale);
		newScale.Z = max(newScale.Z, minimumScale);

		targetObject->SetScale(newScale);
	}
	else // ETranslationType::Rotation
	{
		// [수정된 부분]
		// 1. 현재 마우스 위치를 회전 평면상에서 계산
		FVector currentPoint = FindCirclePlaneIntersection(mouseRay, movementPlane);

		// 2. 드래그 시작점에서 현재점까지의 이동 벡터를 계산
		FVector mouseMoveVec = currentPoint - dragRotationStartPoint;

		// 3. 마우스 이동 벡터를 처음에 설정한 '기준 방향'으로 투영하여 실제 이동 거리를 계산
		float draggedDistance = mouseMoveVec.Dot(dragRotationStartVector);

		// 4. 거리를 각도로 변환 (rotationSensitivity 값으로 회전 속도 조절)
		const float rotationSensitivity = 0.5f;
		float angle = -draggedDistance * rotationSensitivity;

		// 5. 계산된 각도를 적용하여 최종 회전값 계산
		FQuaternion finalQuaternion;
		FVector rotationAxis = GetAxisVector(selectedAxis);

		if (isWorldSpace)
		{
			finalQuaternion = dragStartQuaternion.RotatedWorldAxisAngle(rotationAxis, angle);
		}
		else
		{
			finalQuaternion = dragStartQuaternion.RotatedLocalAxisAngle(rotationAxis, angle);
		}

		targetObject->SetQuaternion(finalQuaternion);
	}
}

void UGizmoManager::EndDrag()
{
	isDragging = false;
	selectedAxis = EAxis::None;
	if (gridPrimitive)
	{
		gridPrimitive->bIsSelected = false;
	}
}

void UGizmoManager::SetTranslationType(ETranslationType type)
{
	if (isDragging)
	{
		return;
	}

	translationType = type;
}

void UGizmoManager::SetGizmoSpace(bool isWorld)
{
	if (isDragging)
	{
		return;
	}

	isWorldSpace = isWorld;
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

FVector UGizmoManager::GetRotationVector(EAxis axis)
{
	switch (axis)
	{
	case EAxis::X:
		return { 0,0,1 };
	case EAxis::Y:
		return { 1,0,0 };
	case EAxis::Z:
		return { 0,1,0 };
	default:
		return { 0,0,0 };
	}
}