#pragma once
#include <optional>
#include "UCamera.h"
#include "UInputManager.h"
#include "UPrimitiveComponent.h"
#include "UPlaneComp.h"
#include "FVertexPosColor.h"
#include "UEngineSubsystem.h"

struct FVector;

struct FRay
{
	FVector MousePos;
	FVector Origin;
	FVector Direction;
};

class URaycastManager : UEngineSubsystem
{
	DECLARE_UCLASS(URaycastManager, UEngineSubsystem)
public:
	static URaycastManager& GetInstance()
	{
		static URaycastManager Instance; // 스택(static storage) 싱글톤
		return Instance;
	}
	URaycastManager(URenderer* InRenderer, UInputManager* InInputManager);
	URaycastManager();
	~URaycastManager();

	bool Initialize(URenderer* InRenderer, UInputManager* InInputManager)
	{
		if (!InRenderer || !InInputManager) return false;
		Renderer = InRenderer;
		InputManager = InInputManager;
		return true;
	}

	void SetRenderer(URenderer* InRenderer) { Renderer = InRenderer; }
	void SetInputManager(UInputManager* InInputManager) { InputManager = InInputManager; }

	template <typename T>
	bool RayIntersectsMeshes(UCamera* Camera, TArray<T*>& Components, T*& HitComponent, FVector& OutImpactPoint);

	TOptional<FVector> RayIntersectsTriangle(FVector TriangleVertices[3]);

	FRay CreateRayFromScreenPosition(UCamera* Camera);

private:
	URenderer* Renderer;
	UInputManager* InputManager;

	float MouseX, MouseY;
	FVector RayOrigin, RayDirection;

	FVector GetRaycastOrigin(UCamera* Camera);
	FVector GetRaycastDirection(UCamera* Camera);

	FVector TransformVertexToWorld(const FVertexPosColor4& Vertex, const FMatrix& WorldMatrix);
};
