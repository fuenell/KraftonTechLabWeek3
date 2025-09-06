#include "stdafx.h"
#include "USceneComponent.h"

FMatrix USceneComponent::GetWorldTransform()
{
    return FMatrix::TRS(RelativeLocation, RelativeRotation, RelativeScale3D);
}
