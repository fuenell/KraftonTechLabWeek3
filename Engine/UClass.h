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
	static inline TArray<TUniquePtr<UClass>> classList;
	//static inline TMap<FString, uint32> nameToId;
	// DisplayName은 typeName을 나타내는 사용자 정의 메타데이터다.
	static inline TMap<FString, uint32> displayNameToId;
	static inline uint32 registeredCount = 0;

	TMap<FString, FString> metadata;
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

	static UClass* GetClass(uint32 typeId)
	{
		return (typeId < classList.size()) ? classList[typeId].get() : nullptr;
	}

	static UClass* FindClass(const FName& Name)
	{
		FName key(Name);

		for (const TUniquePtr<UClass>& Class : classList)
		{
			if (key == Class.get()->className)
				return Class.get();
		}

		return nullptr;

		/*auto it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;*/
	}

	static UClass* FindClassWithDisplayName(const FString& name)
	{
		// 1) DisplayName lookup
		auto it = displayNameToId.find(name);
		if (it != displayNameToId.end())
			return GetClass(it->second);

		// 2) className fallback
		return FindClass(name);
	}


	static const TArray<TUniquePtr<UClass>>& GetClassList()
	{
		return classList;
	}

	UClass();

	bool IsChildOrSelfOf(UClass* baseClass) const
	{
		/*FName Parent = superClassTypeName;
		FName Child = className;

		UClass* Temp = superClass;

		while (className != baseClass->className)
		{
			if (Temp->className == baseClass->className) {
				return true;
			}

			Temp = superClass->superClass;
		}

		return false;*/

		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	const FString& GetUClassName() const { return className.ToString(); }

	const FString& GetDisplayName() const
	{
		auto itr = metadata.find("DisplayName");
		if (itr != metadata.end())
		{
			return itr->second;
		}

		return GetUClassName();
	}

	void SetMeta(const FString& key, const FString& value)
	{
		metadata[key] = value;

		if (key == "DisplayName")
		{
			displayNameToId[value] = typeId;  // typeId는 인스턴스 멤버
		}
	}


	const FString& GetMeta(const FString& key) const
	{
		static FString empty;
		auto it = metadata.find(key);
		return (it != metadata.end()) ? it->second : empty;
	}

	UObject* CreateDefaultObject() const
	{
		return createFunction ? createFunction() : nullptr;
	}

};


