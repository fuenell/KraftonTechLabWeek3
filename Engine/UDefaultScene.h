#pragma once
#include "UScene.h"
#include "USphereComp.h"
#include "UGizmoGridComp.h"
class UDefaultScene :
    public UScene
{
private:
    USphereComp* sphere;
    UGizmoGridComp* gizmoGrid;
public:
    void Update(float deltaTime) override;
    bool OnInitialize() override;
};

