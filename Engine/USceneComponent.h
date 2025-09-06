#pragma once
#include "Matrix.h"
#include "UObject.h"
#include "Vector.h"

class USceneComponent : public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;

    FMatrix GetWorldTransform();
};
