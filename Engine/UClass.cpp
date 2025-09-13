﻿#include "stdafx.h"

UClass::UClass() : className(""), superClassTypeName("") {}

UClass* UClass::RegisterToFactory(const FName& typeName, const TFunction<UObject* ()>& createFunction, const FName& superClassTypeName)
{
	TUniquePtr<UClass> classType = MakeUnique<UClass>();

	//classType->typeId = registeredCount++;
	classType->className = typeName;
	classType->superClassTypeName = superClassTypeName;
	classType->createFunction = createFunction;


	//nameToId[typeName] = classType->typeId;

	UClass* rawPtr = classType.get();
	classList[classType->className] = std::move(classType);
	return rawPtr;
}

void UClass::ResolveTypeBitsets()
{
	for (std::pair<const FName, TUniquePtr<UClass>>& _class : classList)
	{
		const FName& Name = _class.first;
		UClass* Object = _class.second.get();

		if (Object->superClassTypeName != FName(""))
		{
			Object->superClass = FindClass(Object->superClassTypeName);
		}
	}
	for (std::pair<const FName, TUniquePtr<UClass>>& _class : classList)
	{
		UClass* Object = _class.second.get();

		if (Object->processed) continue;

		Object->ResolveTypeBitset(Object);
	}
}

void UClass::ResolveTypeBitset(UClass* classPtr)
{
	TArray<UClass*> stack;
	stack.push_back(classPtr);

	while (!stack.empty())
	{
		UClass* cur = stack.back();

		// 부모가 아직 처리되지 않았다면 먼저 스택에 push
		while (cur->superClass && !cur->superClass->processed)
		{
			stack.push_back(cur->superClass);
			cur = stack.back();
		}

		// 현재 노드 처리
		cur->typeBitset.Clear();
		if (cur->superClass) cur->typeBitset |= cur->superClass->typeBitset;
		cur->typeBitset.Set(cur->typeId);  // 자신의 비트 추가
		cur->processed = true;

		stack.pop_back();
	}
}
