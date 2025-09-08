#pragma once
#include "UPrimitiveComponent.h"

class UMeshManager; // 전방 선언
class URenderer;
class UPrimitiveComponent;

class UGizmoManager
{
public:
	UGizmoManager();

	~UGizmoManager();

	bool Initialize(UMeshManager* meshManager);

	// 기즈모가 조작할 대상 객체를 설정합니다.
	void SetTarget(UPrimitiveComponent* target);

	//// 매 프레임 호출되어 입력을 처리하고 기즈모의 상태를 갱신합니다.
	//void Update(const InputManager& input, const Camera& camera, float deltaTime);

	// 매 프레임 호출되어 기즈모를 화면에 그립니다.
	void Draw(URenderer& renderer);

private:
	UMeshManager* MeshManager;
	UPrimitiveComponent* targetObject = nullptr; // 현재 선택된 객체

	// 역할에 따라 프리미티브를 분리해서 저장합니다.
	UPrimitiveComponent* gridPrimitive = nullptr;              // 월드 그리드
	TArray<UPrimitiveComponent*> transformGizmoPrimitives; // 이동/회전/크기 기즈모
};