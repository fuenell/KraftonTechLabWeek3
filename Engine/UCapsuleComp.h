#pragma once
#include "stdafx.h"
#include "UPrimitiveComponent.h"

class UCapsuleComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UCapsuleComp, UPrimitiveComponent)

private:
	static inline FString type = "Capsule";
};