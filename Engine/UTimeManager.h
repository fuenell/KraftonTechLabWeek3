﻿#pragma once
#include "stdafx.h"
#include "UEngineSubSystem.h"

class UTimeManager : UEngineSubsystem
{
	DECLARE_UCLASS(UTimeManager, UEngineSubsystem)
private:
	// High precision timing
	LARGE_INTEGER frequency;
	LARGE_INTEGER startTime;
	LARGE_INTEGER lastFrameTime;

	// Time tracking
	double deltaTime;
	double totalTime;

	// Frame rate management
	int32 targetFPS;
	double targetFrameTime;

public:
	static UTimeManager& GetInstance()
	{
		static UTimeManager Instance; // 스택(static storage) 싱글톤
		return Instance;
	}
	UTimeManager();
	~UTimeManager();

	// Initialization
	bool Initialize(int32 fps = 60);

	// Frame timing
	void BeginFrame();
	void EndFrame();
	void WaitForTargetFrameTime();

	// Basic getters
	double GetDeltaTime() const { return deltaTime; }
	double GetTotalTime() const { return totalTime; }
	int32 GetTargetFPS() const { return targetFPS; }

	// Basic settings
	void SetTargetFPS(int32 fps);
};