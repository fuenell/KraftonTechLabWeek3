#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObject.h"
#include "FName.h"
#include <memory>

class UClass
{
public:
	static UClass* RegisterToFactory(const FName& TypeName,
		const TFunction<UObject* ()>& CreateFunction, const FName& SuperClassTypeName);

	static void ResolveTypeBitsets();
	void ResolveTypeBitset(UClass* ClassPtr);

	static UClass* FindClass(const FName& Name)
	{
		FName key(Name);

		if (ClassList.count(Name))
			return ClassList[Name].get();
		else
			return nullptr;
	}

	static const TMap<FName, TUniquePtr<UClass>>& GetClassPool()
	{
		return ClassList;
	}

	UClass() {}

	// 현재 클래스가 BaseClass를 상속받았는지 확인한다 (같은 경우 false 반환)
	bool IsChildOf(UClass* BaseClass) const
	{
		const UClass* CurrentClass = SuperClass; // 자기 자신은 제외하고 부모부터 탐색

		while (CurrentClass != nullptr)
		{
			if (CurrentClass->ClassName == BaseClass->ClassName)
			{
				return true;
			}

			CurrentClass = CurrentClass->SuperClass;
		}

		return false;
	}

	// 현재 클래스가 BaseClass이거나, BaseClass를 상속받았는지 확인한다
	bool IsChildOrSelfOf(UClass* BaseClass) const
	{
		if (ClassName == BaseClass->ClassName)
		{
			return true;
		}

		return IsChildOf(BaseClass);
	}

	FName GetUClassName() const { return ClassName; }

	void SetMeta(const FString& Key, const FString& Value)
	{
		Metadata[Key] = Value;
	}

	FName GetMeta(const FString& Key) const
	{
		try
		{
			return Metadata.at(FName(Key));
		}
		catch (const std::out_of_range&)
		{
			return FString("");
		}
	}

	UObject* CreateDefaultObject() const
	{
		return CreateFunction ? CreateFunction() : nullptr;
	}

private:
	static inline TMap<FName, TUniquePtr<UClass>> ClassList{};
	static inline uint32 RegisteredCount = 0;

	TMap<FName, FName> Metadata{};
	uint32 TypeId{};
	FDynamicBitset TypeBitset{};
	FName ClassName{}, SuperClassTypeName{};
	UClass* SuperClass{};
	TFunction<UObject* ()> CreateFunction{};
	bool bIsProcessed = false;
};
