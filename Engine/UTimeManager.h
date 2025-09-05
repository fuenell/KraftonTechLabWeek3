#pragma once
#include "stdafx.h"

class UTimeManager
{
private:
    // High precision timing
    LARGE_INTEGER frequency;
    LARGE_INTEGER startTime;
    LARGE_INTEGER lastFrameTime;

    // Time tracking
    double deltaTime;
    double totalTime;

    // Frame rate management
    int targetFPS;
    double targetFrameTime;

public:
    UTimeManager();
    ~UTimeManager();

    // Initialization
    bool Initialize(int fps = 60);

    // Frame timing
    void BeginFrame();
    void EndFrame();
    void WaitForTargetFrameTime();

    // Basic getters
    double GetDeltaTime() const { return deltaTime; }
    double GetTotalTime() const { return totalTime; }
    int GetTargetFPS() const { return targetFPS; }

    // Basic settings
    void SetTargetFPS(int fps);
};