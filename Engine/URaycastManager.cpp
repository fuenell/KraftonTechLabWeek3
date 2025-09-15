#include "stdafx.h"
#include "URaycastManager.h"
#include "ImguiConsole.h"
#include "Vector.h"
#include "URenderer.h"
#include "UScene.h"

IMPLEMENT_UCLASS(URaycastManager, UEngineSubsystem)

URaycastManager::URaycastManager()
	: Renderer(nullptr),
	InputManager(nullptr),
	RayOrigin(FVector(0, 0, 0)),
	RayDirection(FVector(0, 0, 0)),
	MouseX(0),
	MouseY(0)
{
}

URaycastManager::URaycastManager(URenderer* InRenderer, UInputManager* InInputManager)
	: Renderer(InRenderer),
	InputManager(InInputManager),
	RayOrigin(FVector(0, 0, 0)),
	RayDirection(FVector(0, 0, 0)),
	MouseX(0),
	MouseY(0)
{
}

URaycastManager::~URaycastManager()
{
	Renderer = nullptr;
	InputManager = nullptr;
}

FVector URaycastManager::GetRaycastOrigin(UCamera* Camera)
{
	return Camera->GetLocation();
}

FVector URaycastManager::GetRaycastDirection(UCamera* Camera)
{
	float CameraFOV = Camera->GetFOV();

	// convert the mouse coords to Normalized Device Coordinates (NDC)
	int32 Width = 0, Height = 0;
	Renderer->GetBackBufferSize(Width, Height);
	float NdcX = (2.0f * MouseX) / static_cast<float>(Width) - 1.0f;
	float NdcY = 1.0f - (2.0f * MouseY) / static_cast<float>(Height);

	// get the camera-space ray direction
	// assume the camera is 1.0f units of distance away from the screen
	float Aspect = static_cast<float>(Width) / static_cast<float>(Height);
	FVector RayViewDir;
	RayViewDir.X = NdcX * tan(CameraFOV / 2.0f) * Aspect;
	RayViewDir.Y = NdcY * tan(CameraFOV / 2.0f);
	RayViewDir.Z = -1.0f; // points forward in Camera space
	RayViewDir.Normalize();

	// convert the camera-space ray direction to world direction
	FMatrix V = FMatrix::LookAtLH(Camera->GetLocation(), Camera->GetLocation() + Camera->GetForward(), Camera->GetUp());
	V = FMatrix::Inverse(V);
	FVector4 RayDirection = FMatrix::MultiplyVector(V, FVector4(RayViewDir.X, RayViewDir.Y, RayViewDir.Z, 0.0f));

	return { RayDirection.X, RayDirection.Y, RayDirection.Z };
}

template <typename T>
bool URaycastManager::RayIntersectsMeshes(UCamera* Camera, TArray<T*>& Components, T*& HitComponent, FVector& OutImpactPoint)
{
	MouseX = static_cast<float>(InputManager->GetMouseX());
	MouseY = static_cast<float>(InputManager->GetMouseY());

	FRay Ray = CreateRayFromScreenPosition(Camera);
	RayOrigin = Ray.Origin;
	RayDirection = Ray.Direction;

	bool bHasHit = false;
	float ClosestHitSquared = FLT_MAX; // 거리 비교를 위해 제곱 값을 사용
	T* ClosestComponent = nullptr;

	for (T* Component : Components)
	{
		UMesh* Mesh = Component->GetMesh();
		if (!Mesh || Mesh->NumVertices < 3) continue;	// 버텍스가 3개 미만인 Mesh는 충돌 검사 거부

		FMatrix WorldTransform = Component->GetWorldTransform();

		// 메시에 속한 모든 정점을 월드 좌표로 미리 변환
		TArray<FVector> WorldSpaceVertices;
		WorldSpaceVertices.reserve(Mesh->NumVertices);
		for (int32 i = 0; i < Mesh->NumVertices; ++i)
		{
			WorldSpaceVertices.emplace_back(TransformVertexToWorld(Mesh->Vertices[i], WorldTransform));
		}

		// 삼각형 단위로 충돌 검사
		const int32 NumTriangles = (Mesh->IndexBuffer && Mesh->NumIndices > 0) ? (Mesh->NumIndices / 3) : (Mesh->NumVertices / 3);

		for (int32 TriIndex = 0; TriIndex < NumTriangles; ++TriIndex)
		{
			FVector TriangleVertices[3];

			// 인덱스 버퍼 사용 여부에 따라 정점 구성
			if (Mesh->IndexBuffer && Mesh->NumIndices > 0)
			{
				TriangleVertices[0] = WorldSpaceVertices[Mesh->Indices[TriIndex * 3 + 0]];
				TriangleVertices[1] = WorldSpaceVertices[Mesh->Indices[TriIndex * 3 + 1]];
				TriangleVertices[2] = WorldSpaceVertices[Mesh->Indices[TriIndex * 3 + 2]];
			}
			else
			{
				TriangleVertices[0] = WorldSpaceVertices[TriIndex * 3 + 0];
				TriangleVertices[1] = WorldSpaceVertices[TriIndex * 3 + 1];
				TriangleVertices[2] = WorldSpaceVertices[TriIndex * 3 + 2];
			}

			// Ray와 삼각형의 충돌 검사
			if (auto Result = RayIntersectsTriangle(TriangleVertices))
			{
				// 거리의 제곱을 사용하여 가장 가까운 지점인지 비교
				const float DistSquared = (*Result - Ray.Origin).LengthSquared();
				if (DistSquared < ClosestHitSquared)
				{
					ClosestHitSquared = DistSquared;
					bHasHit = true;
					ClosestComponent = Component;
					OutImpactPoint = *Result;
				}
			}
		}
	}

	if (bHasHit)
	{
		HitComponent = ClosestComponent;
	}
	return bHasHit;
}

template bool URaycastManager::RayIntersectsMeshes<UGizmoComponent>(UCamera*, TArray<UGizmoComponent*>&, UGizmoComponent*&, FVector&);
template bool URaycastManager::RayIntersectsMeshes<UPrimitiveComponent>(UCamera*, TArray<UPrimitiveComponent*>&, UPrimitiveComponent*&, FVector&);


TOptional<FVector> URaycastManager::RayIntersectsTriangle(FVector TriangleVertices[3])
{
	constexpr float Epsilon = std::numeric_limits<float>::epsilon();

	FVector Edge1 = TriangleVertices[1] - TriangleVertices[0];
	FVector Edge2 = TriangleVertices[2] - TriangleVertices[0];
	FVector RayCrossE2 = RayDirection.Cross(Edge2);
	float Det = Edge1.Dot(RayCrossE2);

	if (Det > -Epsilon && Det < Epsilon)
		return {};    // This ray is parallel to this triangle.

	float InvDet = 1.0f / Det;
	FVector S = RayOrigin - TriangleVertices[0];
	float U = InvDet * S.Dot(RayCrossE2);

	if ((U < 0 && abs(U) > Epsilon) || (U > 1 && abs(U - 1) > Epsilon))
		return {};

	FVector s_cross_e1 = S.Cross(Edge1);
	float V = InvDet * RayDirection.Dot(s_cross_e1);

	if ((V < 0 && abs(V) > Epsilon) || (U + V > 1 && abs(U + V - 1) > Epsilon))
		return {};

	// At this stage we can compute t to find out where the intersection point is on the line.
	float T = InvDet * Edge2.Dot(s_cross_e1);

	if (T > Epsilon) // ray intersection
	{
		return FVector(RayOrigin + RayDirection * T);
	}
	else // This means that there is a line intersection but not a ray intersection.
		return {};
}

FVector URaycastManager::TransformVertexToWorld(const FVertexPosColor4& Vertex, const FMatrix& WorldMatrix)
{
	FVector4 pos4(Vertex.x, Vertex.y, Vertex.z, Vertex.w);
	FVector4 worldPos4 = FMatrix::MultiplyVectorRow(pos4, WorldMatrix);
	return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}

// 현재 클릭한 마우스 위치와 카메라를 기준으로 ray 생성
FRay URaycastManager::CreateRayFromScreenPosition(UCamera* Camera)
{
	MouseX = static_cast<float>(InputManager->GetMouseX());
	MouseY = static_cast<float>(InputManager->GetMouseY());

	int32 ViewportWidth = 0, ViewportHeight = 0;
	Renderer->GetBackBufferSize(ViewportWidth, ViewportHeight);

	// 1단계: Screen -> NDC
	FVector ndcPos;
	ndcPos.X = (MouseX / ViewportWidth) * 2.0f - 1.0f;
	ndcPos.Y = 1.0f - (MouseY / ViewportHeight) * 2.0f;
	ndcPos.Z = 1.0f; // Far Plane

	FMatrix invProjectionMatrix = FMatrix::Inverse(Camera->GetProj());
	FVector viewPos = invProjectionMatrix.TransformPointRow(ndcPos);

	// 3단계: View -> World
	FMatrix invViewMatrix = FMatrix::Inverse(Camera->GetView());
	FVector worldPos = invViewMatrix.TransformPointRow(viewPos);

	if (Camera->IsOrtho())
	{
		FRay resultRay;
		resultRay.Origin = worldPos;
		resultRay.Direction = Camera->GetForward();
		return resultRay;
	}

	// 4단계: 방향 벡터 계산
	FRay resultRay;
	resultRay.Origin = Camera->GetLocation(); // 카메라의 월드 위치
	resultRay.Direction = (worldPos - resultRay.Origin).Normalized();
	resultRay.MousePos = { MouseX, MouseY };
	//UE_LOG("%f %f %f / %f %f %f", resultRay.Origin.X, resultRay.Origin.Y, resultRay.Origin.Z, resultRay.Direction.X, resultRay.Direction.Y, resultRay.Direction.Z);

	return resultRay;
}