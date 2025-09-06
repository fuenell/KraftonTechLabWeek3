#pragma once
#include "UEngineStatics.h"
#include "Globals.h"
#include "ISerializable.h"

typedef int int32;
typedef unsigned int uint32;

class UObject : ISerializable
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

    virtual bool CountOnInspector() {
        return false;
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

    json::JSON Serialize() const override
    {
        return json::JSON();
    }

    bool Deserialize(const json::JSON& data) override
    {
        return true;
    }
};