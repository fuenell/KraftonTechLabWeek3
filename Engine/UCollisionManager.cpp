#include "stdafx.h"
#include "UCollisionManager.h"
#include "UPrimitiveComponent.h"

IMPLEMENT_UCLASS(UCollisionManager, UEngineSubsystem)


static FORCEINLINE float MaxFloat(float A, float B) { return (A > B) ? A : B; }
static FORCEINLINE float MinFloat(float A, float B) { return (A < B) ? A : B; }

static FORCEINLINE FVector MakeMaxPerAxis(const FVector& L, const FVector& R)
{
	return FVector(MaxFloat(L.X, R.X), MaxFloat(L.Y, R.Y), MaxFloat(L.Z, R.Z));
}

static FORCEINLINE FVector MakeMinPerAxis(const FVector& L, const FVector& R)
{
	return FVector(MinFloat(L.X, R.X), MinFloat(L.Y, R.Y), MinFloat(L.Z, R.Z));
}



bool UCollisionManager::Initialize()
{
	return true;
}

void UCollisionManager::Update()
{
	if (!CollisionObjects || !PickedPrimitive)
	{
		bHasOverlap = false;
		LastHit = nullptr;
		return;
	}

	FVector Center;
	UPrimitiveComponent* Hit = nullptr;

	//겹친애 있으면 중점과 primitivecomponent를 가져옴
	if (GetFirstOverlapCenter(CollisionObjects, PickedPrimitive, Center, Hit))
	{
		bHasOverlap = true;
		LastOverlapCenter = Center;
		LastHit = Hit;
	}
	else
	{
		bHasOverlap = false;
		LastHit = nullptr;
	}
}

bool UCollisionManager::CheckCollide()
{
	return false;
}

void UCollisionManager::SetObjects(TArray<USceneComponent*>* InObject, UPrimitiveComponent* InPickedPrimitive)
{
	CollisionObjects = InObject;
	PickedPrimitive = InPickedPrimitive;
}

void UCollisionManager::Release()
{
}

bool UCollisionManager::GetOverlapCenter(const FBounds& A, const FBounds& B, FVector& OutCenter)
{
	// 교집합 AABB 구간
	const FVector InterMin = MakeMaxPerAxis(A.Min, B.Min);
	const FVector InterMax = MakeMinPerAxis(A.Max, B.Max);

	// 한 축이라도 비겹침이면 실패
	if (InterMin.X > InterMax.X || InterMin.Y > InterMax.Y || InterMin.Z > InterMax.Z)
	{
		return false;
	}

	OutCenter = (InterMin + InterMax) * 0.5f;
	return true;
}

bool UCollisionManager::GetFirstOverlapCenter(TArray<USceneComponent*>* InObjects, UPrimitiveComponent* InPickedPrimitive, FVector& OutCenter, UPrimitiveComponent*& OutHit)
{
	OutHit = nullptr;
	if (!InObjects || !InPickedPrimitive) { return false; }

	const FBounds Picked = InPickedPrimitive->GetBoundingBox();

	const int32 Count = static_cast<int32>(InObjects->size());
	for (int32 i = 0; i < Count; ++i)
	{
		USceneComponent* Comp = (*InObjects)[i];
		if (!Comp || Comp == InPickedPrimitive) { continue; }

		// USceneComponent에 기본 false, UPrimitiveComponent에서 override true 라고 했으므로 안전 필터
		if (!Comp->IsManageable()) { continue; }

		UPrimitiveComponent* Other = static_cast<UPrimitiveComponent*>(Comp);

		FVector Center;
		if (GetOverlapCenter(Picked, Other->GetBoundingBox(), Center))
		{
			OutCenter = Center;
			OutHit = Other; // “처음” 겹친 대상
			return true;
		}
	}
	return false;
}
