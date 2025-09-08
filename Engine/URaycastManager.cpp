#include "stdafx.h"
#include "URaycastManager.h"

#include "ImguiConsole.h"
#include "Vector.h"
#include "URenderer.h"
#include "UScene.h"

URaycastManager::URaycastManager()
    : Renderer(nullptr),
      InputManager(nullptr),
      RayOrigin(FVector(0,0,0)),
      RayDirection(FVector(0,0,0)),
      MouseX(0),
      MouseY(0)
{
}

URaycastManager::URaycastManager(URenderer* renderer, UCamera* camera, UInputManager* inputManager)
    : Renderer(renderer),
      InputManager(inputManager),
      RayOrigin(FVector(0,0,0)),
      RayDirection(FVector(0,0,0)),
      MouseX(0),
      MouseY(0)
{
}

URaycastManager::~URaycastManager()
{
    Renderer = nullptr;
    InputManager   = nullptr;
}

FVector URaycastManager::GetRaycastOrigin(UCamera* camera)
{
    return camera->GetLocation();
}

FVector URaycastManager::GetRaycastDirection(UCamera* camera)
{
    float CameraFOV = camera->GetFOV();

    // convert the mouse coords to Normalized Device Coordinates (NDC)
    int width = 0, height = 0;
    Renderer->GetBackBufferSize(width, height);
    float ndcX = (2.0f * MouseX) / static_cast<float>(width) - 1.0f;
    float ndcY = 1.0f - (2.0f * MouseY) / static_cast<float>(height);

    // get the camera-space ray direction
    // assume the camera is 1.0f units of distance away from the screen
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    FVector rayViewDir;
    rayViewDir.X = ndcX * tan(CameraFOV / 2.0f) * aspect;
    rayViewDir.Y = ndcY * tan(CameraFOV / 2.0f);
    rayViewDir.Z = -1.0f; // points forward in Camera space
    rayViewDir.Normalize();

    // convert the camera-space ray direction to world direction
    FMatrix V = FMatrix::LookAtRH(camera->GetLocation(), camera->GetLocation() + camera->GetForward(), camera->GetUp());
    V = FMatrix::Inverse(V);
    FVector4 rayDirection = FMatrix::MultiplyVector(V, FVector4(rayViewDir.X, rayViewDir.Y, rayViewDir.Z, 0.0f));
    
    return {rayDirection.X, rayDirection.Y, rayDirection.Z};
}

template <typename T>
bool URaycastManager::RayIntersectsMeshes(UCamera* camera, TArray<T*>& components, T*& hitComponent)
{
    MouseX = static_cast<float>(InputManager->GetMouseX());
    MouseY = static_cast<float>(InputManager->GetMouseY());

    RayOrigin = GetRaycastOrigin(camera);
    RayDirection = GetRaycastDirection(camera);
    
    bool hit = false;
    float closestHit = FLT_MAX;
    T* closestComponent = nullptr;
    
    for (T* component : components)
    {
        UMesh* mesh = component->GetMesh();
        FMatrix worldTransform = component->GetWorldTransform();

        std::cout << "\nChecking component: " << component
                  << " | NumVertices: " << mesh->NumVertices << std::endl;
        
        if (mesh->NumVertices < 3) continue;
        
        for (int i = 0; i + 2 < mesh->NumVertices; i += 3)
        {
            FVector triangleVertices[3] = {
                TransformVertexToWorld(mesh->Vertices[i], worldTransform),
                TransformVertexToWorld(mesh->Vertices[i + 1], worldTransform),
                TransformVertexToWorld(mesh->Vertices[i + 2], worldTransform)
            };
        
            auto result = RayIntersectsTriangle(triangleVertices);
            if (result.has_value())
            {
                float t = (*result - RayOrigin).Length(); // distance along ray
                std::cout << "    -> Hit at distance " << t << std::endl;
                if (t < closestHit)
                {
                    closestHit = t;
                    std::cout << "    -> Closest so far (was " << closestHit << ")" << std::endl;
                    hit = true;
                    closestComponent = component;
                }
            }
        }
    }

    if (hit)
    {
        hitComponent = closestComponent;
    }
    return hit;
}

template bool URaycastManager::RayIntersectsMeshes<UGizmoComponent>(UCamera*, TArray<UGizmoComponent*>&, UGizmoComponent*&);
template bool URaycastManager::RayIntersectsMeshes<UPrimitiveComponent>(UCamera*, TArray<UPrimitiveComponent*>&, UPrimitiveComponent*&);


std::optional<FVector> URaycastManager::RayIntersectsTriangle(FVector triangleVertices[3])
{
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    
    FVector edge1 = triangleVertices[1] - triangleVertices[0];
    FVector edge2 = triangleVertices[2] - triangleVertices[0];
    FVector ray_cross_e2 = RayDirection.Cross(edge2);
    float det = edge1.Dot(ray_cross_e2);

    if (det > -epsilon && det < epsilon)
        return {};    // This ray is parallel to this triangle.

    float inv_det = 1.0f / det;
    FVector s = RayOrigin - triangleVertices[0];
    float u = inv_det * s.Dot(ray_cross_e2);

    if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u-1) > epsilon))
        return {};

    FVector s_cross_e1 = s.Cross(edge1);
    float v = inv_det * RayDirection.Dot(s_cross_e1);

    if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
        return {};

    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = inv_det * edge2.Dot(s_cross_e1);

    if (t > epsilon) // ray intersection
    {
        return FVector(RayOrigin + RayDirection * t);
    }
    else // This means that there is a line intersection but not a ray intersection.
        return {};
}

FVector URaycastManager::TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world)
{
    FVector4 pos4(vertex.x, vertex.y, vertex.z, vertex.w);
    FVector4 worldPos4 = FMatrix::MultiplyVector(world, pos4);
    return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}

// bool RaycastManager::RayIntersectsSphere(FVector& rayOrigin, FVector& rayDirection, USphereComp& sphere, float& tHit)
// {
//     // R(t) = O + tD
//     // where O = rayOrigin, D = rayDirection
//     // C = intersection of circle
//     
//     // to check intersection:
//     // || R(t) - C ||^2 = r^2
//     // because || R(t) - C || is the distance from point to center. squared for easier calculations
//     // || O + tD - C ||^2 = r^2
//     // Let L = O - C
//     // || L + tD ||^2 = r^2
//     // (L + tD) . (L + tD) = r^2
//     // t^2 (D . D) + 2(D . L)t + (L . L - r^2) = 0
//     // note D . D = 1 because D is normalized
//     // use the quadratic equation
//     // t = - (D . L) +- sqrt((D . L)^2 - (L . L - r^2))
//
//     FVector L = rayOrigin - sphere.GetPosition();
//     float b = L.Dot(rayDirection);
//     float c = L.Dot(L) - sphere.GetRadius() * sphere.GetRadius();
//
//     float determinant = b * b - c;
//     if (determinant < 0.0f) return false;
//
//     tHit = - b - sqrtf(determinant);
//     // may be negative, which will be the intersection behind the ray origin.
//     // doesn't matter for now
//     return true;
// }