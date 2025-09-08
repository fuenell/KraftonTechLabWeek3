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

    void SetRenderer(URenderer* renderer) { Renderer = renderer; }
    void SetCamera(UCamera* camera) { Camera = camera; }
    void SetInputManager(UInputManager* inputManager) { InputManager = inputManager; }

    // TODO: remove this and call RayIntersectsMesh elsewhere
    void Update();

    // bool RayIntersectsMesh(UPrimitiveComponent& primitive, float& tHit);
    bool RayIntersectsMesh(UMesh* mesh, const FMatrix& worldTransform, float& tHit);
    std::optional<FVector>  RayIntersectsTriangle(FVector triangleVertices[3]);
    
private:
    URenderer* Renderer;
    UCamera* Camera;
    UInputManager* InputManager;

    float MouseX;
    float MouseY;
    FVector RayOrigin, RayDirection;

    FVector GetRaycastOrigin();
    FVector GetRaycastDirection();
    
    FVector TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world);
};