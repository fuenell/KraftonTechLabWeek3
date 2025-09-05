#pragma once
typedef int int32;
typedef unsigned int uint32;

class UEngineStatics
{
public:
    static uint32 GenUUID()
    {
        return NextUUID++;
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
    
private:
    static uint32 NextUUID;
    static uint32 TotalAllocationBytes;
    static uint32 TotalAllocationCount;
};