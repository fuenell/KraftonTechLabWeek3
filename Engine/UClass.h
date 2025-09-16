#pragma once
#include "UEngineStatics.h"
#include "json.hpp"
#include "UObject.h"
#include "FName.h"
#include <memory>

class UClass
{
public:
	// 클래스 시스템을 위한 정적 인터페이스 함수들입니다.
	static UClass* RegisterToFactory(const FName& TypeName, const TFunction<UObject* ()>& CreateFunction, const FName& SuperClassTypeName);
	static void Init();
	static UClass* FindClass(const FName& Name);
	static const TMap<FName, TUniquePtr<UClass>>& GetClassPool();

	// 생성자입니다.
	UClass();

	// 객체 인스턴스를 생성하는 함수입니다.
	UObject* CreateDefaultObject() const;

	// 클래스 계층 구조와 타입을 확인하는 함수들입니다.
	bool IsChildOf(UClass* BaseClass) const;
	bool IsChildOrSelfOf(UClass* BaseClass) const;

	// 클래스 정보를 가져오는 Getter 함수입니다.
	FName GetUClassName() const;

	// 메타데이터를 관리하는 함수들입니다.
	void SetMeta(const FString& Key, const FString& Value);
	FName GetMeta(const FString& Key) const;

private:
	// 'Construct on First Use' 관용구를 사용하여 정적 초기화 문제를 해결합니다.
	static TMap<FName, TUniquePtr<UClass>>& GetClassList();

private:
	// 클래스 관련 메타데이터를 저장하는 맵입니다.
	TMap<FName, FName> Metadata{};
	// 이 클래스의 이름입니다.
	FName ClassName{};
	// 부모 클래스의 이름입니다. (초기화 단계에서 사용됩니다)
	FName SuperClassTypeName{};
	// 부모 클래스에 대한 포인터입니다.
	UClass* SuperClass{};
	// 이 클래스의 인스턴스를 생성하는 팩토리 함수입니다.
	TFunction<UObject* ()> CreateFunction{};
};
