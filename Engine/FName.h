#pragma once
#include <string>

#include "UEngineStatics.h"

struct FName
{
    FName(char* pStr);
    FName(FString str);
    FName() { GetOrCreateName(""); }

    int32 Compare(const FName& Other) const;
    bool operator==(const FName& Other) const;
    bool operator!=(const FName& Other) const;
    operator FString() const;

    void GetOrCreateName(FString);
    FString ToString() const;

    int32 DisplayIndex;
    int32 ComparisonIndex;


    static TMap<FString, int32> CaseInsensitiveFNamesWithStringKey;
    static TMap<int32, FString> CaseInsensitiveFNamesWithIndexKey;

    static TMap<FString, int32> CaseSensitiveFNamesStringKey;
    static TMap<int32, FString> CaseSensitiveFNamesIndexKey;

    static int32 DisplayIndexCount;
    static int32 ComparisonIndexCount;
};

// TMap에서의 사용을 위한 hash<FName> 오버로딩
namespace std {
    template<>
    struct hash<FName> {
        size_t operator()(const FName& name) const noexcept {
            return std::hash<int>()(name.ComparisonIndex);
        }
    };
}
