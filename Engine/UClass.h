#pragma once
#include "UEngineStatics.h"
#include "FDynamicBitset.h"
#include "json.hpp"
#include "UObject.h"
#include <memory>

class UClass : public UObject
{
private:
	static inline std::vector<std::unique_ptr<UClass>> classList;
	static inline std::unordered_map<std::string, uint32> nameToId;
	static inline uint32 registeredCount = 0;

	FDynamicBitset typeBitset;
	std::string className;
	const UClass* superClass;
	std::function<UObject*()> createFunction;
public:
	static UClass* RegisterToFactory(const std::string& typeName, const std::function<UObject*()>& createFunction, const UClass* const superClass = nullptr
		//,std::function<json::JSON()>& serializeFunction, std::function<void(const json::JSON&)>& deserializeFunction
	)
	{
		std::unique_ptr<UClass> classType = std::make_unique<UClass>();

		classType->typeId = registeredCount++;
		classType->className = typeName;
		classType->superClass = superClass;
		classType->createFunction = createFunction;

		classType->typeBitset.Set(classType->typeId);

		if(superClass)
			classType->typeBitset |= superClass->typeBitset;

		nameToId[typeName] = classType->typeId;

		UClass* rawPtr = classType.get();
		classList.push_back(std::move(classType));
		return rawPtr;
	}

	uint32 GetTypeId() { return typeId; }

};

