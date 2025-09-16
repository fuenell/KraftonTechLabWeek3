#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "UInputManager.h"
#include "UGUI.h"
#include "UTimeManager.h"
#include "UMeshManager.h"
#include "URaycastManager.h"
#include "USceneManager.h"
#include "UGizmoManager.h"
#include "ULineBatcherManager.h"
#include "UUIDRenderer.h"
#include "UTextureManager.h"
#include "ConfigManager.h"
#include "TestUUIDRenderer.h"
#include "USpriteManager.h"
#include "SpriteRenderer.h"

class Application
{
public:
	Application();
	virtual ~Application();

	// Application lifecycle
	bool Initialize(HINSTANCE HInstance, const std::wstring& Title = L"Engine Application",
		int32 Width = 1024, int32 Height = 768);
	void Run();
	void Shutdown();

	// 순수 가상 함수
	virtual void Update(float DeltaTime) = 0;
	virtual void Render() = 0;

	// System access
	URenderer& GetRenderer() { return Renderer; }
	UMeshManager& GetMeshManager() { return MeshManager; }
	USceneManager& GetSceneManager() { return SceneManager; }
	UInputManager& GetInputManager() { return InputManager; }
	UGUI& GetGUI() { return Gui; }
	UTimeManager& GetTimeManager() { return TimeManager; }
	URaycastManager& GetRaycastManager() { return RaycastManager; }
	ULineBatcherManager& GetLineBatcherManager() { return LineBatcherManager; }

	// Window management
	HWND GetWindowHandle() const { return HWnd; }

	// Application state
	bool IsRunning() const { return bIsRunning; }
	void RequestExit() { bIsRunning = false; }
	virtual void OnSceneChange() {}

protected:
	// Internal methods
	bool CreateMainWindow(HINSTANCE HInstance);
	void ProcessMessages();
	void InternalUpdate();
	void InternalRender();

	// Window procedure
	static LRESULT CALLBACK WndProc(HWND HWnd, UINT Message, WPARAM WParam, LPARAM LParam);

	// Override point32s for derived classes
	virtual bool OnInitialize() = 0;
	virtual void RenderGUI() {}
	virtual void OnShutdown() {}
	// Called on window resize
	virtual void OnResize(int32 Width, int32 Height) {}

protected:
	// Core systems
	HWND HWnd;
	URenderer Renderer;
	UInputManager InputManager;
	UGUI Gui;
	UTimeManager TimeManager;
	UMeshManager MeshManager;
	USceneManager SceneManager;
	URaycastManager RaycastManager;
	ULineBatcherManager LineBatcherManager;


	UUIDRenderer UUIDRenderer;
	USpriteManager SpriteManager;

	// Application state
	bool bIsRunning;
	bool bIsInitialized;

	// Window properties
	static WCHAR WindowClass[];
	static WCHAR DefaultTitle[];

	std::wstring WindowTitle;
	// 화면 사이즈 조절
	bool bIsSizing = false;
	int32 WindowWidth;
	int32 WindowHeight;
};