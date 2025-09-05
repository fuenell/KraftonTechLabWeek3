#pragma once
#include "stdafx.h"
#include "UApplication.h"
#include "USphere.h"
#include "Camera.h"

// Simple application that inherits from UApplication
class SimpleApplication : public UApplication
{
private:
    USphere* sphere, *sphere2;
    ID3D11Buffer* sphereVertexBuffer;
	// 화면의 너비, 높이
	int width, height;
    UCamera camera;

public:
    SimpleApplication() = default;
    ~SimpleApplication() = default;

protected:
    void Update(float deltaTime) override;
    void Render() override;
    bool OnInitialize() override;
};