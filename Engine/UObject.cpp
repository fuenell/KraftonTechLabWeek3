#include "stdafx.h"
#include "UObject.h"

TArray<UObject*> UObject::GUObjectArray;
TArray<uint32> UObject::FreeIndices;
uint32 UObject::NextFreshIndex;