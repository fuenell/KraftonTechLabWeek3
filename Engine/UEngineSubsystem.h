﻿#pragma once
#include "UObject.h"

class UEngineSubsystem : public UObject
{
	DECLARE_UCLASS(UEngineSubsystem, UObject)
public:
	UEngineSubsystem();
	virtual bool Initialize() { return true; }  // 나중에 필요시 오버라이드
	virtual void Shutdown() {}
};

