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
	static inline uint32 registeredCount = 0;

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

	bool IsChildOf(UClass* baseClass) const {
		return baseClass && typeBitset.Test(baseClass->typeId);
	}

	const std::string& GetClassName() const { return className; }

	static UClass* FindClass(const std::string& name) {
		auto it = nameToId.find(name);
		return (it != nameToId.end()) ? GetClass(it->second) : nullptr;
	}

	UObject* CreateDefaultObject() const {
		return createFunction ? createFunction() : nullptr;
	}

};


