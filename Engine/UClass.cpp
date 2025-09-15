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

// 모든 클래스의 SuperClass를 지정한다 
// RegisterToFactory에서 등록되지 않은 Class를 SuperClass를 설정할 경우를 고려해서 한번에 처리
void UClass::Init()
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
}