#include "stdafx.h"
#include "URaycastManager.h"
#include "Vector.h"
#include "URenderer.h"

void URaycastManager::Update(UInputManager& input, USphereComp& primitive)
{
    if (!input.IsMouseButtonDown(0)) return;
    
    MouseX = static_cast<float>(input.GetMouseX());
    MouseY = static_cast<float>(input.GetMouseY());

    RayOrigin = GetRaycastOrigin();
    RayDirection = GetRaycastDirection();

    float tHit;
    if (RayIntersectsMesh(primitive, tHit))
    {
        std::cout << tHit;
    }
    
    // USphereComp* closestSphere = nullptr;
    // float closestT = FLT_MAX;
    //
    // // Optional: do something with the hit sphere
    // if (closestSphere)
    // {
    //     FVector hitPoint = RayOrigin + RayDirection * closestT;
    //
    //     std::cout << "Hit sphere at (" 
    //       << hitPoint.X << ", " 
    //       << hitPoint.Y << ", " 
    //       << hitPoint.Z << ")" 
    //       << std::endl;
    // }
}

FVector URaycastManager::GetRaycastOrigin()
{
    return Camera.GetLocation();
}

FVector URaycastManager::GetRaycastDirection()
{
    float CameraFOV = Camera.GetFOV();

    // convert the mouse coords to Normalized Device Coordinates (NDC)
    int width = 0, height = 0;
    Renderer.GetBackBufferSize(width, height);
    float ndcX = (2.0f * MouseX) / static_cast<float>(width) - 1.0f;
    float ndcY = 1.0f - (2.0f * MouseY) / static_cast<float>(height);

    // get the camera-space ray direction
    // assume the camera is 1.0f units of distance away from the screen
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    FVector rayViewDir;
    rayViewDir.X = ndcX * tan(CameraFOV / 2.0f) * aspect;
    rayViewDir.Y = ndcY * tan(CameraFOV / 2.0f);
    rayViewDir.Z = 1.0f; // points forward in Camera space
    rayViewDir.Normalize();

    // convert the camera-space ray direction to world direction
    FMatrix V = Camera.GetView();
    V = FMatrix::Inverse(V);
    FVector4 rayDirection = FMatrix::MultiplyVector(V, FVector4(rayViewDir.X, rayViewDir.Y, rayViewDir.Z, 0.0f));

    return {rayDirection.X, rayDirection.Y, rayDirection.Z};
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

bool URaycastManager::RayIntersectsMesh(USphereComp& sphere, float& tHit)
{
    if (sphere.GetMesh().NumVertices < 3)
        return false;

    // Map vertex buffer
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HRESULT hr = Renderer.GetDeviceContext()->Map(sphere.GetMesh().VertexBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
    if (FAILED(hr)) return false;

    FVertexSimple* vertices = reinterpret_cast<FVertexSimple*>(mappedData.pData);
    bool hit = false;
    float closestT = FLT_MAX;

    FMatrix worldTransform = sphere.GetWorldTransform();

    for (int i = 0; i < sphere.GetMesh().NumVertices; i += 3)
    {
        FVector triangleVertices[3] = {
            TransformVertexToWorld(vertices[i], worldTransform),
            TransformVertexToWorld(vertices[i + 1], worldTransform),
            TransformVertexToWorld(vertices[i + 2], worldTransform)
        };

        auto result = RayIntersectsTriangle(triangleVertices);
        if (result.has_value())
        {
            float t = (*result - RayOrigin).Length(); // distance along ray
            if (t < closestT)
            {
                closestT = t;
                hit = true;
            }
        }
    }

    Renderer.GetDeviceContext()->Unmap(sphere.GetMesh().VertexBuffer, 0);

    if (hit)
    {
        tHit = closestT;
        return true;
    }

    return false;
}


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

FVector URaycastManager::TransformVertexToWorld(const FVertexSimple& vertex, const FMatrix& world)
{
    FVector4 pos4(vertex.x, vertex.y, vertex.z, 1.0f);
    FVector4 worldPos4 = FMatrix::MultiplyVector(world, pos4);
    return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}