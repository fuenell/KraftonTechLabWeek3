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

class Application
{
protected:
	// Core systems
	HWND hWnd;
	URenderer renderer;
	UInputManager inputManager;
	UGUI gui;
	UTimeManager timeManager;
	UMeshManager meshManager;
	USceneManager sceneManager;
	URaycastManager raycastManager;
	ULineBatcherManager LineBatcherManager;

	// Application state
	bool bIsRunning;
	bool bIsInitialized;

	// Window properties
	static WCHAR WindowClass[];
	static WCHAR DefaultTitle[];

	std::wstring windowTitle;
	// 화면 사이즈 조절
	bool isSizing = false;
	int32 windowWidth;
	int32 windowHeight;



public:
	Application();
	virtual ~Application();

	// Application lifecycle
	bool Initialize(HINSTANCE hInstance, const std::wstring& title = L"Engine Application",
		int32 width = 1024, int32 height = 768);
	void Run();
	void Shutdown();

	// 순수 가상 함수
	virtual void Update(float deltaTime) = 0;
	virtual void Render() = 0;

	// System access
	URenderer& GetRenderer() { return renderer; }
	UMeshManager& GetMeshManager() { return meshManager; }
	USceneManager& GetSceneManager() { return sceneManager; }
	UInputManager& GetInputManager() { return inputManager; }
	UGUI& GetGUI() { return gui; }
	UTimeManager& GetTimeManager() { return timeManager; }
	URaycastManager& GetRaycastManager() { return raycastManager; }
	ULineBatcherManager& GetLineBatcherManager() { return LineBatcherManager; }

	// Window management
	HWND GetWindowHandle() const { return hWnd; }

	// Application state
	bool IsRunning() const { return bIsRunning; }
	void RequestExit() { bIsRunning = false; }
	virtual UScene* CreateDefaultScene();
	virtual void OnSceneChange() {}

protected:
	// Internal methods
	bool CreateMainWindow(HINSTANCE hInstance);
	void ProcessMessages();
	void InternalUpdate();
	void InternalRender();

	// Window procedure
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Override point32s for derived classes
	virtual bool OnInitialize();
	virtual void RenderGUI() {}
	virtual void OnShutdown() {}
	// Called on window resize
	virtual void OnResize(int32 width, int32 height) {}
};