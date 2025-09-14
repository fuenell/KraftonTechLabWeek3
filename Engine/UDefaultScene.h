#pragma once
#include "UScene.h"
#include "USphereComp.h"
#include "UGizmoGridComp.h"
class UDefaultScene :
    public UScene
{
private:
    static inline bool IsFirstTime = true;
public:
    void Update(float DeltaTime) override;
    bool OnInitialize() override;
};

 