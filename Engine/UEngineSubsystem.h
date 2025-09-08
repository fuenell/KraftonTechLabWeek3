#pragma once
#include "UObject.h"

class UEngineSubsystem : public UObject
{
public:
    virtual bool Initialize() { return true; }  // 나중에 필요시 오버라이드
    virtual void Shutdown() {}
};

