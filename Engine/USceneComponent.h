#pragma once
#include "UObject.h"
#include "Vector.h"

class USceneComponent : public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;
};
