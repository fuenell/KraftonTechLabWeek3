#pragma once
#include <optional>
#include "UCamera.h"
#include "UInputManager.h"
#include "UPrimitiveComponent.h"
#include "USphereComp.h"
#include "UTriangleComp.h"
#include "FVertexPosColor.h"

class URaycastManager
{
public:
    URaycastManager();
    URaycastManager(URenderer* renderer, UCamera* camera, UInputManager* inputManager);
    ~URaycastManager();

    bool Initialize(URenderer* renderer, UInputManager* inputManager)
    {
        if (!renderer || !inputManager) return false;
        Renderer = renderer;
        InputManager = inputManager;
        return true;
    }

    void SetRenderer(URenderer* renderer) { Renderer = renderer; }
    void SetInputManager(UInputManager* inputManager) { InputManager = inputManager; }

    // TODO: remove this and call RayIntersectsMesh elsewhere
    void Update(UCamera* camera);

    // bool RayIntersectsMesh(UPrimitiveComponent& primitive, float& tHit);
    // bool RayIntersectsMeshes(UCamera* camera, TArray<FRaycastTarget> targets);
    template <typename T>
    bool RayIntersectsMeshes(UCamera* camera, TArray<T*>& components, T*& hitComponent);
    
    std::optional<FVector> RayIntersectsTriangle(FVector triangleVertices[3]);

private:
    URenderer* Renderer;
    UInputManager* InputManager;

    float MouseX, MouseY;
    FVector RayOrigin, RayDirection;

    FVector GetRaycastOrigin(UCamera* camera);
    FVector GetRaycastDirection(UCamera* camera);

    FVector TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world);
};
