// USceneComponent.cpp
#include "stdafx.h"
#include "USceneComponent.h"

#define JSON_HEADER
#include "json.hpp" // 이 cpp에서만 정의 생성

json::JSON USceneComponent::Serialize() const {
    json::JSON result;
    result["Location"] = json::Array(RelativeLocation.X, RelativeLocation.Y, RelativeLocation.Z);
    result["Rotation"] = json::Array(RelativeRotation.X, RelativeRotation.Y, RelativeRotation.Z);
    result["Scale"] = json::Array(RelativeScale3D.X, RelativeScale3D.Y, RelativeScale3D.Z);
    result["Type"] = GetType();
    return result;
}
