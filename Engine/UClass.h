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
	static inline TMap<FName, FName> displayNameToId;
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

	/*static UClass* GetClass(uint32 typeId)
	{
		return (typeId < classList.size()) ? classList[typeId].get() : nullptr;
	}*/

	static UClass* FindClass(const FName& Name)
	{
		FName key(Name);

		/*for (const TUniquePtr<UClass>& Class : classList)
		{
			if (key == Class.get()->className)
				return Class.get();
		}

		return nullptr;*/

		if (classList.count(Name))
			return classList[Name].get();
		else
			return nullptr;
	}

	static UClass* FindClassWithDisplayName(const FString& Name)
	{
		//// 1) DisplayName lookup
		//auto it = displayNameToId.find(name);
		//if (it != displayNameToId.end())
		//	return GetClass(it->second);

		//// 2) className fallback
		//return FindClass(name);

		if (!displayNameToId.count(Name))
			return nullptr;
		
		const FName& Key = displayNameToId[Name];
		return FindClass(Key);
	}


	static const TMap<FName, TUniquePtr<UClass>>& GetClassPool()
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

	FName GetUClassName() const { return className; }

	FName GetDisplayName() const
	{
		FString Name = GetMeta("DIsplayName");
		if (Name == "")
			return GetUClassName();

		return Name;
	}

	void SetMeta(const FString& key, const FString& value)
	{
		metadata[key] = value;

		if (key == "DisplayName")
		{
			displayNameToId[value] = className;  // typeId는 인스턴스 멤버
		}
	}


	FName GetMeta(const FString& key) const
	{
		try {
			return metadata.at(FName(key));
		}
		catch (const std::out_of_range&) {
			return FString("");
		}
	}

	UObject* CreateDefaultObject() const
	{
		return createFunction ? createFunction() : nullptr;
	}

};


