#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphereComp.h"
#include "URaycastManager.h"

// Simple application that inherits from UApplication
class SimpleApplication : public UApplication
{
private:
public:
    SimpleApplication() = default;
    ~SimpleApplication() = default;
    UScene* CreateDefaultScene() override;

protected:
    void Update(float deltaTime) override;
    void Render() override;
    void RenderGUI() override;
    bool OnInitialize() override;
    void OnResize(int width, int height) override;
};