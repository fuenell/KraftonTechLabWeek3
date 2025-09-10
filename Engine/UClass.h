#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObject.h"
#include <memory>

class UClass : public UObject
{
	DECLARE_UCLASS(UClass, UObject)
private:
	static inline std::vector<std::unique_ptr<UClass>> classList;
	static inline std::unordered_map<std::string, uint32> nameToId;
	static inline std::unordered_map<std::string, uint32> displayNameToId;
	static inline uint32 registeredCount = 0;

	std::unordered_map<std::string, std::string> metadata;
	uint32 typeId;
	FDynamicBitset typeBitset;
	std::string className, superClassTypeName;
	UClass* superClass;
	std::function<UObject*()> createFunction;
	bool processed = false;
public:
	static UClass* RegisterToFactory(const std::string& typeName, 
		const std::function<UObject* ()>& createFunction, const std::string& superClassTypeName);

	static void ResolveTypeBitsets();
	void ResolveTypeBitset(UClass* classPtr);

	static UClass* GetClass(uint32 typeId) {
		return (typeId < classList.size()) ? classList[typeId].get() : nullptr;
	}

	static UClass* FindClass(const std::string& name) {
		auto it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}

	static UClass* FindClassWithDisplayName(const std::string& name)
	{
		// 1) DisplayName lookup
		auto it = displayNameToId.find(name);
		if (it != displayNameToId.end())
			return GetClass(it->second);

		// 2) className fallback
		it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}


	static const TArray<std::unique_ptr<UClass>>& GetClassList()
	{
		return classList;
	}

	bool IsChildOrSelfOf(UClass* baseClass) const {
		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	const std::string& GetUClassName() const { return className; }

	const std::string& GetDisplayName() const
	{
		auto itr = metadata.find("DisplayName");
		if (itr != metadata.end())
		{
			return itr->second;
		}

		return GetUClassName();
	}

	void SetMeta(const std::string& key, const std::string& value)
	{
		metadata[key] = value;

		if (key == "DisplayName")
		{
			displayNameToId[value] = typeId;  // typeId는 인스턴스 멤버
		}
	}


	const std::string& GetMeta(const std::string& key) const {
		static std::string empty;
		auto it = metadata.find(key);
		return (it != metadata.end()) ? it->second : empty;
	}

	UObject* CreateDefaultObject() const {
		return createFunction ? createFunction() : nullptr;
	}

};


