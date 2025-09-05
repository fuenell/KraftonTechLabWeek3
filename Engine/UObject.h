#pragma once
#include "UEngineStatics.h"
typedef int int32;
typedef unsigned int uint32;
template <typename T>
using TArray = std::vector<T>;

// class UObject;
// TArray<UObject*> GUObjectArray;

class UObject
{
public:
    static TArray<UObject*> GUObjectArray; // TEMP
    
    UObject()
    {
        UUID = UEngineStatics::GenUUID();
        InternalIndex = static_cast<uint32>(GUObjectArray.size());
        GUObjectArray.push_back(this);
    }

    virtual ~UObject()
    {
        // Remove from GUObjectArray (optional: compact array)
        // For now, just mark as nullptr
        GUObjectArray[InternalIndex] = nullptr;
    }

    // Override new/delete for tracking
    void* operator new(size_t size)
    {
        UEngineStatics::AddAllocation(size);
        return ::operator new(size);
    }

    void operator delete(void* ptr, size_t size)
    {
        UEngineStatics::RemoveAllocation(size);
        ::operator delete(ptr);
    }
    
    uint32 UUID;
    uint32 InternalIndex;
};