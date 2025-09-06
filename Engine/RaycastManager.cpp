#include "stdafx.h"
#include "RaycastManager.h"
#include "Vector.h"
#include "URenderer.h"

void RaycastManager::Update(UInputManager& input)
{
    if (input.IsMouseButtonDown(0))
    {
        int mouseX = input.GetMouseX();
        int mouseY = input.GetMouseY();
    }
}

FVector RaycastManager::GetRaycastOrigin(FVector camera)
{
    return {camera.X, camera.Y, camera.Z};
}

FVector RaycastManager::GetRaycastDirection(float mouseX, float mouseY)
{
    // TODO: replace these with real camera info once the camera class is created
    float cameraFOV = 90.0f;
    FVector target(0.0f, 0.0f, 0.0f);
    FVector up(0, 0, 1);
    FVector eye(10.f, 0.0f, 0.0f);

    // convert the mouse coords to Normalized Device Coordinates (NDC)
    int width = 0, height = 0;
    Renderer.GetBackBufferSize(width, height);
    float ndcX = (2.0f * mouseX) / static_cast<float>(width) - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / static_cast<float>(height);

    // get the camera-space ray direction
    // assume the camera is 1.0f units of distance away from the screen
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    FVector rayViewDir;
    rayViewDir.X = ndcX * tan(cameraFOV / 2.0f) * aspect;
    rayViewDir.Y = ndcY * tan(cameraFOV / 2.0f);
    rayViewDir.Z = 1.0f; // points forward in camera space
    rayViewDir.Normalize();

    // convert the camera-space ray direction to world direction
    FMatrix V = FMatrix::LookAtLHRow(eye, target, up);
    V = FMatrix::Inverse(V);
    FVector4 rayDirection = FMatrix::MultiplyVector(V, FVector4(rayViewDir.X, rayViewDir.Y, rayViewDir.Z, 0.0f));

    return {rayDirection.X, rayDirection.Y, rayDirection.Z};
}

// WIP
bool RaycastManager::RayIntersectsSphere(FVector& rayOrigin, FVector& rayDirection, USphereComp& sphere, float& tHit)
{
    // R(t) = O + tD
    // where O = rayOrigin, D = rayDirection
    // C = intersection of circle
    
    // to check intersection:
    // || R(t) - C ||^2 = r^2
    // || O + tD - C ||^2 = r^2
    // Let L = O - C
    // || L + tD ||^2 = r^2
    // (L + tD) . (L + tD) = r^2
    // t^2 (D . D) + 2(D . L)t + (L . L - r^2) = 0
    // note D . D = 1 because D is normalized
    // use the quadratic equation
    FVector L = rayOrigin - sphere.GetPosition();
    float b = L.Dot(rayDirection);
    float c = L.Dot(L);

    // float determinant = 
    
    return false;
}