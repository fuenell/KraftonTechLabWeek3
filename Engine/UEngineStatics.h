﻿#pragma once
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef std::string FString;

// Unreal Engine 스타일 스마트 포인터 별칭 정의
// TSharedPtr - shared_ptr 래핑
template<typename T>
using TSharedPtr = std::shared_ptr<T>;

// TWeakPtr - weak_ptr 래핑  
template<typename T>
using TWeakPtr = std::weak_ptr<T>;

// TUniquePtr - unique_ptr 래핑
template<typename T>
using TUniquePtr = std::unique_ptr<T>;

// MakeShared 헬퍼 함수 (Unreal의 MakeShareable과 유사)
template<typename T, typename... Args>
TSharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

// MakeUnique 헬퍼 함수 
template<typename T, typename... Args>
TUniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename U>
using TMap = std::unordered_map<T, U>;

template<typename T, typename U>
using TPair = std::pair<T, U>;

template<typename T>
using TQueue = std::queue<T>;

template<typename T>
using TFunction = std::function<T>;

template<typename T>
using TOptional = std::optional<T>;

class UEngineStatics
{
public:
    static uint32 GenUUID()
    {
        if (!isEnabled) return UINT_MAX;
        return NextUUID++;
    }

    static uint32 GetNextUUID() { return NextUUID; }
    static void SetNextUUID(uint32 nextUUID) { 
        NextUUID = nextUUID;
    }

    static void AddAllocation(size_t size)
    {
        TotalAllocationBytes += static_cast<uint32>(size);
        TotalAllocationCount++;
    }

    static void RemoveAllocation(size_t size)
    {
        TotalAllocationBytes -= static_cast<uint32>(size);
        TotalAllocationCount--;
    }

    static uint32 GetTotalAllocationBytes() { return TotalAllocationBytes; }
    static uint32 GetTotalAllocationCount() { return TotalAllocationCount; }

    static void SetUUIDGeneration(bool enabled)
    {
        isEnabled = enabled;        
    }
    
private:
    static uint32 NextUUID;
    static uint32 TotalAllocationBytes;
    static uint32 TotalAllocationCount;
    static bool isEnabled;
};