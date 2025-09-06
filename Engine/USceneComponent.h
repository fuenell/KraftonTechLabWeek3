#pragma once
#include "UObject.h"
#include "Vector.h"

class USceneComponent : public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;

    virtual std::string GetType() const = 0;
    json::JSON Serialize() const override;
};
