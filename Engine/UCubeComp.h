﻿#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UPrimitiveComponent.h"
#include "FVertexPosColor.h"
#include "Vector.h"

class UCubeComp : public UPrimitiveComponent
{
	DECLARE_UCLASS(UCubeComp, UPrimitiveComponent)
private:
	static inline FString type = "Cube";

	static USceneComponent* Create()
	{
		USceneComponent* newInstance = new UCubeComp();

		return newInstance;
	}

	bool IsManageable() override { return true; }

public:
	UCubeComp(FVector pos = { 0, 0, 0 }, FVector rot = { 0, 0, 0 }, FVector scl = { 1, 1, 1 })
		:UPrimitiveComponent(pos, rot, scl)
	{
	}
};