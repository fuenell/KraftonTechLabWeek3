#include "stdafx.h"
#include "MemoryTracker.h"
#include "UEngineStatics.h"

void* operator new(size_t size)
{
	// 1. malloc으로 실제 메모리를 먼저 할당합니다.
	void* ptr = std::malloc(size);
	if (!ptr)
	{
		// 할당 실패 시 new의 표준 동작인 예외를 발생시킵니다.
		throw std::bad_alloc();
	}

	// 2. 할당에 성공한 후에만 추적 정보를 기록합니다. (안전성)
	UEngineStatics::AddAllocation(size);
	return ptr;
}

void operator delete(void* ptr, size_t size) noexcept
{
	// 1. 추적 정보를 먼저 제거합니다.
	UEngineStatics::RemoveAllocation(size);

	// 2. free로 실제 메모리를 해제합니다.
	std::free(ptr);
}
