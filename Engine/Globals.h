#pragma once
#include <vector>
template <typename T>
using TArray = std::vector<T>;

class UObject;

extern TArray<UObject*> GUObjectArray;