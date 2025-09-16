#pragma once
#include "UEngineSubsystem.h"

class USceneComponent;
class UPrimitiveComponent;

class UCollisionManager : UEngineSubsystem
{
    DECLARE_UCLASS(UCollisionManager, UEngineSubsystem)

public:
    static UCollisionManager& GetInstance()
    {
        static UCollisionManager Instance; // 스택(static storage) 싱글톤
        return Instance;
    }


public:
    // USceneManager::GetInstance().GetScene()
    bool Initialize();
    void Update();


    bool CheckCollide();

    void SetObjects(TArray<USceneComponent*>* InObject, UPrimitiveComponent* InPickedPrimitive);

    void Release();


    FORCEINLINE bool HasOverlap() const { return bHasOverlap; }
    FORCEINLINE const FVector& GetLastOverlapCenter() const { return LastOverlapCenter; }
    FORCEINLINE UPrimitiveComponent* GetLastHit() const { return LastHit; }


private:
    UCollisionManager()
    {
    }
    ~UCollisionManager()
    {
    }



    // 겹침이면 OutCenter = 교집합 AABB의 중심, 반환값 true
    static bool GetOverlapCenter(const FBounds& A, const FBounds& B, FVector& OutCenter);

    // 필요 시: Picked vs 목록에서 첫 번째로 겹친 대상의 중심을 얻는 헬퍼
    bool GetFirstOverlapCenter(TArray<USceneComponent*>* InObjects, UPrimitiveComponent* InPickedPrimitive, FVector& OutCenter, UPrimitiveComponent*& OutHit);




private:

    TArray<USceneComponent*>* CollisionObjects{};
    UPrimitiveComponent* PickedPrimitive{};

    bool bHasOverlap = false;
    FVector LastOverlapCenter = FVector::Zero();
    UPrimitiveComponent* LastHit = nullptr;
};
