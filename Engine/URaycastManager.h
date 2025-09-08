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
    URaycastManager() : MouseX(0), MouseY(0) {}

    bool Initialize(URenderer* renderer);
    
    void Update(UCamera* camera, UInputManager& input); // parameter for testing
    FVector GetRaycastOrigin(UCamera * camera);
    FVector GetRaycastDirection(UCamera * camera);

    bool RayIntersectsMesh(UPrimitiveComponent& primitive, float& tHit);
    std::optional<FVector>  RayIntersectsTriangle(FVector triangleVertices[3]);
    
private:
    URenderer* Renderer;

    float MouseX, MouseY;
    FVector RayOrigin, RayDirection;

    FVector TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world);
};
