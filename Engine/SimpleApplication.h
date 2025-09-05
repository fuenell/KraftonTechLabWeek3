#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphere.h"
// Simple application that inherits from UApplication
class SimpleApplication : public UApplication
{
private:
    USphere* sphere;
    ID3D11Buffer* sphereVertexBuffer;
public:
    SimpleApplication() = default;
    ~SimpleApplication() = default;

protected:
    void Update(float deltaTime) override;
    void Render() override;
    void RenderGUI() override;
    bool OnInitialize() override;
};