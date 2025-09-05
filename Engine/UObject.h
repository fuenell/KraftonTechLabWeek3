#pragma once
#include "UEngineStatics.h"
#include "Globals.h"
typedef int int32;
typedef unsigned int uint32;

class UObject
{
public:
    uint32 UUID;
    uint32 InternalIndex;
    
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
};