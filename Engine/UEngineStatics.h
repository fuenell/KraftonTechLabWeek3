#pragma once
typedef int int32;
typedef unsigned int uint32;
typedef std::string FString;

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