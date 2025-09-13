#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObject.h"
#include "FName.h"
#include <memory>

class UClass
{
private:
	static inline TMap<FName, TUniquePtr<UClass>> classList;
	//static inline TMap<FString, uint32> nameToId;
	// DisplayName은 typeName을 나타내는 사용자 정의 메타데이터다.
	static inline TMap<FName, FName> TypeNameToId;
	static inline uint32 registeredCount = 0;

	TMap<FName, FName> metadata;
	uint32 typeId;
	FDynamicBitset typeBitset;
	FName className, superClassTypeName;
	UClass* superClass;
	TFunction<UObject* ()> createFunction;
	bool processed = false;
public:
	static UClass* RegisterToFactory(const FName& typeName,
		const TFunction<UObject* ()>& createFunction, const FName& superClassTypeName);

	static void ResolveTypeBitsets();
	void ResolveTypeBitset(UClass* classPtr);

	static UClass* FindClass(const FName& Name)
	{
		FName key(Name);

		if (classList.count(Name))
			return classList[Name].get();
		else
			return nullptr;
	}

	static UClass* FindClassWithTypeName(const FString& Name)
	{
		if (!TypeNameToId.count(Name))
			return nullptr;

		const FName& Key = TypeNameToId[Name];
		return FindClass(Key);
	}


	static const TMap<FName, TUniquePtr<UClass>>& GetClassPool()
	{
		return classList;
	}

	UClass();

	bool IsChildOrSelfOf(UClass* baseClass) const
	{
		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	FName GetUClassName() const { return className; }

	FName GetTypeName() const
	{
		FString Name = GetMeta("TypeName");
		if (Name == "")
			return GetUClassName();

		return Name;
	}

	void SetMeta(const FString& key, const FString& value)
	{
		metadata[key] = value;

		if (key == "TypeName")
		{
			TypeNameToId[value] = className;  // typeId는 인스턴스 멤버
		}
	}


	FName GetMeta(const FString& key) const
	{
		try
		{
			return metadata.at(FName(key));
		}
		catch (const std::out_of_range&)
		{
			return FString("");
		}
	}

	UObject* CreateDefaultObject() const
	{
		return createFunction ? createFunction() : nullptr;
	}

};


