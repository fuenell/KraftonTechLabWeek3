#pragma once
#include "UPrimitiveComponent.h"
#include "UGizmoComponent.h"
#include "URaycastManager.h"
#include "UEngineSubsystem.h"

class UMeshManager; // 전방 선언
class URenderer;

enum class EAxis { None, X, Y, Z };

struct FPlane
{
	FVector PointOnPlane;
	FVector Normal;
};

class UGizmoManager : UEngineSubsystem
{
public:
	UGizmoManager();

	~UGizmoManager();

	bool Initialize(UMeshManager* meshManager);

	// 기즈모가 조작할 대상 객체를 설정합니다.
	void SetTarget(UPrimitiveComponent* target);

	bool IsRaycastHit(URaycastManager* rayCastManager, UGizmoComponent& out);

	//// 매 프레임 호출되어 입력을 처리하고 기즈모의 상태를 갱신합니다.
	//void Update(const InputManager& input, const Camera& camera, float deltaTime);

	// 매 프레임 호출되어 기즈모를 화면에 그립니다.
	void Draw(URenderer& renderer);

	UPrimitiveComponent* GetTarget() { return targetObject; }
	TArray<UGizmoComponent*>& GetRaycastableGizmos();
	void BeginDrag(const FRay& mouseRay, EAxis selectedAxis);
	void UpdateDrag(const FRay& mouseRay);
	void EndDrag();

	//bool IsDragging() const { return m_bIsDragging; }

private:
	// 드래그 상태 변수
	bool isDragging = false;
	EAxis selectedAxis = EAxis::None;
	FVector dragOffset;

	UMeshManager* meshManager;
	UPrimitiveComponent* targetObject = nullptr; // 현재 선택된 객체

	// 드래그 계산을 위해 저장해두는 정보
	FVector dragStartLocation;    // 드래그 시작 시 Target의 월드 위치
	FPlane movementPlane;         // 계산된 이동 평면

	// 유틸리티 함수
	FVector GetAxisVector(EAxis axis);

	// 역할에 따라 프리미티브를 분리해서 저장합니다.
	UGizmoComponent* gridPrimitive = nullptr;              // 월드 그리드
	TArray<UGizmoComponent*> transformGizmoPrimitives; // 이동/회전/크기 기즈모

	FVector FindCirclePlaneIntersection(const FRay& mouseRay, const FPlane& plane);
};