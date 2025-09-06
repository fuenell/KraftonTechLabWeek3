#pragma once
#include "UInputManager.h"
#include "UPrimitiveComponent.h"
#include "USphereComp.h"

class RaycastManager
{
public:
    URenderer& Renderer;
    
    void Update(UInputManager& input);
    FVector GetRaycastOrigin(FVector camera); // TODO: replace parameter with camera
    FVector GetRaycastDirection(float mouseX, float mouseY);

    bool RayIntersectsSphere(FVector& rayOrigin, FVector& rayDirection, USphereComp& sphere, float& tHit);
};
