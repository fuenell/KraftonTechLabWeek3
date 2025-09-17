#pragma once
#include "UEngineStatics.h"
#include "TArray.h"
#include "ISerializable.h"
#include <type_traits>
#include "UClass.h"
#include "RTTI.h"
#include "FName.h"

typedef int int32;
typedef unsigned int uint32;

class UObject : public ISerializable
{
	DECLARE_ROOT_UCLASS(UObject)
private:
	static inline TArray<uint32> FreeIndices;
	static inline uint32 NextFreshIndex = 0;
public:
	static inline TArray<UObject*> GUObjectArray;
	uint32 UUID;
	uint32 InternalIndex;
	FName Name;

	UObject() : Name("UObject")
	{
		UUID = UEngineStatics::GenUUID();

		// 방법 1: 빠른 추가 - 재사용 인덱스 우선 사용
		if (!FreeIndices.empty())
		{
			// 삭제된 슬롯 재사용 (O(1))
			InternalIndex = FreeIndices.back();
			FreeIndices.pop_back();
			GUObjectArray[InternalIndex] = this;
		}
		else
		{
			// 새 슬롯 할당 (O(1) amortized)
			InternalIndex = NextFreshIndex++;
			GUObjectArray.push_back(this);
		}
	}

	virtual ~UObject()
	{
		// 방법 1: 빠른 삭제 - nullptr 마킹 + 인덱스 재사용 큐에 추가
		if (InternalIndex < GUObjectArray.size() && GUObjectArray[InternalIndex] == this)
		{
			GUObjectArray[InternalIndex] = nullptr;  // O(1)
			FreeIndices.push_back(InternalIndex);    // O(1)
		}
	}

	virtual bool CountOnInspector()
	{
		return false;
	}

	template<typename T>
	bool IsA() const
	{
		return GetClass()->IsChildOrSelfOf(T::StaticClass());
	}

	template<typename T>
	T* Cast()
	{
		return IsA<T>() ? static_cast<T*>(this) : nullptr;
	}

	template<typename T>
	const T* Cast() const
	{
		return IsA<T>() ? static_cast<const T*>(this) : nullptr;
	}

	json::JSON Serialize() const override
	{
		return json::JSON();
	}

	bool Deserialize(const json::JSON& data) override
	{
		return true;
	}

	void SetUUID(uint32 uuid)
	{
		UUID = uuid;
	}

	static void ClearFreeIndices()
	{
		FreeIndices.clear();
	}
};