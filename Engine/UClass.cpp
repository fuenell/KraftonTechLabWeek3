#include "stdafx.h"
#include "UClass.h"

//================================================================
// Static Implementation
//================================================================

TMap<FName, TUniquePtr<UClass>>& UClass::GetClassList()
{
	static TMap<FName, TUniquePtr<UClass>> ClassList;
	return ClassList;
}

UClass* UClass::RegisterToFactory(const FName& TypeName, const TFunction<UObject* ()>& CreateFunction, const FName& SuperClassTypeName)
{
	TUniquePtr<UClass> ClassType = MakeUnique<UClass>();

	ClassType->ClassName = TypeName;
	ClassType->SuperClassTypeName = SuperClassTypeName;
	ClassType->CreateFunction = CreateFunction;

	UClass* RawPtr = ClassType.get();
	GetClassList().emplace(TypeName, std::move(ClassType));

	return RawPtr;
}

void UClass::Init()
{
	for (auto& ClassEntry : GetClassList())
	{
		UClass* Object = ClassEntry.second.get();

		if (Object->SuperClassTypeName != FName(""))
		{
			Object->SuperClass = FindClass(Object->SuperClassTypeName);
		}
	}
}

UClass* UClass::FindClass(const FName& Name)
{
	if (GetClassList().count(Name))
	{
		return GetClassList().at(Name).get();
	}
	return nullptr;
}

const TMap<FName, TUniquePtr<UClass>>& UClass::GetClassPool()
{
	return GetClassList();
}

//================================================================
// Object Lifetime
//================================================================

UClass::UClass()
{
	// 멤버 변수들은 헤더에서 인-클래스 초기화를 통해 안전하게 초기화됩니다.
}

UObject* UClass::CreateDefaultObject() const
{
	return CreateFunction ? CreateFunction() : nullptr;
}

//================================================================
// Class Hierarchy
//================================================================

// 현재 클래스가 BaseClass를 상속받았는지 확인한다 (같은 경우 false 반환)
bool UClass::IsChildOf(UClass* BaseClass) const
{
	const UClass* CurrentClass = SuperClass;

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
bool UClass::IsChildOrSelfOf(UClass* BaseClass) const
{
	if (ClassName == BaseClass->ClassName)
	{
		return true;
	}
	return IsChildOf(BaseClass);
}

FName UClass::GetUClassName() const
{
	return ClassName;
}

//================================================================
// Metadata
//================================================================

void UClass::SetMeta(const FString& Key, const FString& Value)
{
	Metadata[Key] = Value;
}

FName UClass::GetMeta(const FString& Key) const
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