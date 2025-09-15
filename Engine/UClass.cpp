#include "stdafx.h"

UClass* UClass::RegisterToFactory(const FName& TypeName, const TFunction<UObject* ()>& CreateFunction, const FName& SuperClassTypeName)
{
	TUniquePtr<UClass> ClassType = MakeUnique<UClass>();

	ClassType->ClassName = TypeName;
	ClassType->SuperClassTypeName = SuperClassTypeName;
	ClassType->CreateFunction = CreateFunction;

	UClass* RawPtr = ClassType.get();
	ClassList[ClassType->ClassName] = std::move(ClassType);
	return RawPtr;
}

void UClass::ResolveTypeBitsets()
{
	for (std::pair<const FName, TUniquePtr<UClass>>& ClassEntry : ClassList)
	{
		const FName& Name = ClassEntry.first;
		UClass* Object = ClassEntry.second.get();

		if (Object->SuperClassTypeName != FName(""))
		{
			Object->SuperClass = FindClass(Object->SuperClassTypeName);
		}
	}
	for (std::pair<const FName, TUniquePtr<UClass>>& _class : ClassList)
	{
		UClass* Object = _class.second.get();

		if (Object->bIsProcessed) continue;

		Object->ResolveTypeBitset(Object);
	}
}

void UClass::ResolveTypeBitset(UClass* ClassPtr)
{
	TArray<UClass*> Stack;
	Stack.push_back(ClassPtr);

	while (!Stack.empty())
	{
		UClass* Cur = Stack.back();

		// 부모가 아직 처리되지 않았다면 먼저 스택에 push
		while (Cur->SuperClass && !Cur->SuperClass->bIsProcessed)
		{
			Stack.push_back(Cur->SuperClass);
			Cur = Stack.back();
		}

		// 현재 노드 처리
		Cur->TypeBitset.Clear();
		if (Cur->SuperClass) Cur->TypeBitset |= Cur->SuperClass->TypeBitset;
		Cur->TypeBitset.Set(Cur->TypeId);  // 자신의 비트 추가
		Cur->bIsProcessed = true;

		Stack.pop_back();
	}
}
