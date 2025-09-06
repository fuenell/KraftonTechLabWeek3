#pragma once
#include <optional>
#include "Camera.h"
#include "UInputManager.h"
#include "UPrimitiveComponent.h"
#include "USphereComp.h"
#include "UTriangleComp.h"

class URaycastManager
{
public:
    URaycastManager(URenderer& renderer, UCamera& camera)
    : Renderer(renderer), Camera(camera) 
    {
    }
    
    void Update(UInputManager& input, USphereComp& primitive); // parameter for testing
    FVector GetRaycastOrigin();
    FVector GetRaycastDirection();

    bool RayIntersectsMesh(USphereComp& sphere, float& tHit);
    std::optional<FVector>  RayIntersectsTriangle(FVector triangleVertices[3]);
    
private:
    URenderer& Renderer;
    UCamera& Camera;

    float MouseX, MouseY;
    FVector RayOrigin, RayDirection;

    FVector TransformVertexToWorld(const FVertexSimple& vertex, const FMatrix& world);
};
