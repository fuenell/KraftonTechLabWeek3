#pragma once
#include "stdafx.h"
#include "UEngineSubsystem.h"

// Forward declaration
class UTimeManager;

class UGUI : public UEngineSubsystem
{
	DECLARE_UCLASS(UGUI, UEngineSubsystem)
private:
	bool bInitialized;
	bool bShowDemoWindow;

public:
	static UGUI& GetInstance()
	{
		static UGUI Instance; // 스택(static storage) 싱글톤
		return Instance;
	}
	UGUI();
	~UGUI();

	// Initialization and cleanup
	bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Shutdown();

	// Frame management
	void BeginFrame();
	void EndFrame();
	void Render();

	bool WantCaptureMouse() const;
	bool WantCaptureKeyboard() const;
};