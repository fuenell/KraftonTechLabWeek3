#pragma once
#include <string>

#include "UEngineStatics.h"

struct FName
{
    FName(char* pStr);
    FName(FString str);

    int32 Compare(const FName& Other) const;
    bool operator==(const FName& Other) const;

    void GetOrCreateName(FString);
    FString ToString();

    int32 DisplayIndex;
    int32 ComparisonIndex;


    static TMap<FString, int32> CaseInsensitiveFNamesWithStringKey;
    static TMap<int32, FString> CaseInsensitiveFNamesWithIndexKey;

    static TMap<FString, int32> CaseSensitiveFNamesStringKey;
    static TMap<int32, FString> CaseSensitiveFNamesIndexKey;

    static int32 DisplayIndexCount;
    static int32 ComparisonIndexCount;
};