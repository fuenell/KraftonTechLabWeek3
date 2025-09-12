#include "stdafx.h"
#include "FName.h"

int32 FName::DisplayIndexCount = 0;
int32 FName::ComparisonIndexCount = 0;

TMap<FString, int32> FName::CaseInsensitiveFNamesWithStringKey;
TMap<int32, FString> FName::CaseInsensitiveFNamesWithIndexKey;
TMap<FString, int32> FName::CaseSensitiveFNamesStringKey;
TMap<int32, FString> FName::CaseSensitiveFNamesIndexKey;

FName::FName(char* pStr)
{	
	GetOrCreateName(FString(pStr));
}

FName::FName(FString str)
{
	GetOrCreateName(str);
}

int32 FName::Compare(const FName& Other) const
{
	return this->ComparisonIndex - Other.ComparisonIndex;
}

bool FName::operator==(const FName& Other) const
{
	return this->ComparisonIndex == Other.ComparisonIndex;
}

void FName::GetOrCreateName(FString String)
{
	// Hash 테이블에 값이 있으면 Index에 저장하고 그렇지 않으면 새로 만든다.
	if (CaseSensitiveFNamesStringKey.count(String))
		DisplayIndex = CaseSensitiveFNamesStringKey[String];
	else
	{
		CaseSensitiveFNamesStringKey[String] = DisplayIndexCount;
		DisplayIndex = DisplayIndexCount;
		CaseSensitiveFNamesIndexKey[DisplayIndex] = String;
		DisplayIndexCount++;
	}

	// 모두 소문자로 변경
	for (size_t i = 0; i < String.size(); ++i)
		String[i] = static_cast<char>(std::tolower(static_cast<int32>(String[i])));
	if (CaseInsensitiveFNamesWithStringKey.count(String))
		ComparisonIndex = CaseInsensitiveFNamesWithStringKey[String];
	else
	{
		CaseInsensitiveFNamesWithStringKey[String] = ComparisonIndexCount;
		ComparisonIndex = ComparisonIndexCount;
		CaseInsensitiveFNamesWithIndexKey[ComparisonIndex] = String;
		ComparisonIndex++;
	}
}

FString FName::ToString()
{
	return CaseSensitiveFNamesIndexKey[DisplayIndex];
}