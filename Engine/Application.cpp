#include "stdafx.h"
#include "Application.h"
#include "UScene.h"

// Static member definitions
WCHAR Application::WindowClass[] = L"EngineWindowClass";
WCHAR Application::DefaultTitle[] = L"Engine Application";

// Global application pointer for window procedure
Application* g_pApplication = nullptr;

// Forward declaration for ImGui
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Application::Application()
	: HWnd(nullptr)
	, bIsRunning(false)
	, bIsInitialized(false)
	, WindowTitle(DefaultTitle)
	, WindowWidth(1024)
	, WindowHeight(768)
{
	g_pApplication = this;
}

Application::~Application()
{
	if (bIsInitialized)
	{
		Shutdown();
	}
	g_pApplication = nullptr;
}

bool Application::Initialize(HINSTANCE HInstance, const std::wstring& Title, int32 Width, int32 Height)
{
	if (bIsInitialized)
		return false;

	UClass::Init();

	WindowTitle = Title;
	WindowWidth = Width;
	WindowHeight = Height;

	// 현재 시간을 이용해 난수 시드를 초기화합니다.
	srand(static_cast<unsigned int>(time(NULL)));

	// Create main window
	if (!CreateMainWindow(HInstance))
	{
		return false;
	}

	/*
	URenderer는 Device 및 DeviceContext를 요구하는 객체들보다 무조건 무조건 먼저 Initialize 되어야 한다.
	*/
	URenderer::GetInstance().Initialize(HWnd);
	URenderer::GetInstance().CreateShader();
	URenderer::GetInstance().CreateConstantBuffer();
	
	ID3D11Device* Device = URenderer::GetInstance().GetDevice();
	ID3D11DeviceContext* DeviceContext = URenderer::GetInstance().GetDeviceContext();

	UTimeManager::GetInstance().Initialize();
	UMeshManager::GetInstance().Initialize(&URenderer::GetInstance());
	USceneManager::GetInstance().Initialize(g_pApplication);
	URaycastManager::GetInstance().Initialize(&URenderer::GetInstance(), &UInputManager::GetInstance());
	UGUI::GetInstance().Initialize(HWnd, Device, DeviceContext);
	ULineBatcherManager::GetInstance().Initialize(Device, 1024);
	USpriteManager::GetInstance().Initialize(Device);
	UTextureManager::GetInstance().Initialize(HWnd, Device, DeviceContext);

	if (!USubUVManager::GetInstance().Initialize(
		L"DDS/Explosion_SubUV.dds",
		6,
		4)
		)
	{
		MessageBox(HWnd, L"Failed to initialize SubUVManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}


	/*
	ConfigManager은 반드시 Camera와 LineBatcherManager 및 변수를 받아오는 객체들이
	초기화된 이후에 초기화해야 한다.
	*/
	ConfigManager& ConfigManager = ConfigManager::Instance();
	ConfigManager.RegisterConfigTargets(&ULineBatcherManager::GetInstance());
	ConfigManager.LoadConfig();

	// Allow derived classes to initialize
	if (!OnInitialize())
	{
		return false;
	}

	bIsInitialized = true;
	bIsRunning = true;

	return true;
}

void Application::Run()
{
	if (!bIsInitialized)
		return;

	while (bIsRunning)
	{
		UTimeManager::GetInstance().BeginFrame();




		// 대강 이런식으로 추가 구현 예정
		/*
		for (auto& bb : BBoxes)                                     // 3) 디버그 라인/박스 등
			LineBatcherManager.AddBoundingBox(bb, bbWorld);
		for (auto& dl : DebugLines)
			LineBatcherManager.AddLine(dl.a, dl.b, dl.color);
		*/


		UInputManager::GetInstance().Update();
		ProcessMessages();

		if (!bIsRunning)
			break;

		InternalUpdate();
		InternalRender();

		UTimeManager::GetInstance().EndFrame();
		UTimeManager::GetInstance().WaitForTargetFrameTime();
	}
}

void Application::Shutdown()
{
	if (!bIsInitialized)
		return;

	bIsRunning = false;

	ConfigManager::Instance().SaveConfig();

	// Allow derived classes to cleanup
	OnShutdown();

	// Shutdown core systems
	UGUI::GetInstance().Shutdown();
	URenderer::GetInstance().ReleaseConstantBuffer();
	URenderer::GetInstance().ReleaseShader();
	URenderer::GetInstance().Release();
	ULineBatcherManager::GetInstance().Release();
	UTextureManager::GetInstance().Release();
	USpriteManager::GetInstance().Release();
	USubUVManager::GetInstance().Release();

	bIsInitialized = false;
}

bool Application::CreateMainWindow(HINSTANCE HInstance)
{
	// Register window class
	WNDCLASSW Wndclass = {};
	Wndclass.lpfnWndProc = WndProc;
	Wndclass.hInstance = HInstance;
	Wndclass.lpszClassName = WindowClass;
	Wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	Wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClassW(&Wndclass))
	{
		return false;
	}

	// Calculate window size including borders
	RECT WindowRect = { 0, 0, WindowWidth, WindowHeight };
	AdjustWindowRect(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int32 AdjustedWidth = WindowRect.right - WindowRect.left;
	int32 AdjustedHeight = WindowRect.bottom - WindowRect.top;

	// Create window
	HWnd = CreateWindowExW(
		0,
		WindowClass,
		WindowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		AdjustedWidth, AdjustedHeight,
		nullptr, nullptr, HInstance, nullptr
	);

	if (!HWnd)
	{
		return false;
	}

	ShowWindow(HWnd, SW_SHOW);
	UpdateWindow(HWnd);

	return true;
}

void Application::ProcessMessages()
{
	MSG Msg;
	while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);

		if (Msg.message == WM_QUIT)
		{
			bIsRunning = false;
			return;
		}
	}
}

void Application::InternalUpdate()
{
	float DeltaTime = static_cast<float>(UTimeManager::GetInstance().GetDeltaTime());


	// Call derived class update
	Update(DeltaTime);
}

void Application::InternalRender()
{
	// Prepare rendering
	URenderer::GetInstance().Prepare();
	URenderer::GetInstance().PrepareShader();

	// Call derived class render
	Render();

	// Render GUI
	UGUI::GetInstance().BeginFrame();
	UGUI::GetInstance().Render();
	RenderGUI();
	UGUI::GetInstance().EndFrame();

	// Present the frame
	URenderer::GetInstance().SwapBuffer();
}

LRESULT CALLBACK Application::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Let ImGui handle the message first
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}

	if (g_pApplication)
	{
		// Let input manager process input messages
		UInputManager::GetInstance().ProcessMessage(hWnd, message, wParam, lParam);
		//g_pApplication->ProcessMessages();
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_ENTERSIZEMOVE:
		if (g_pApplication) g_pApplication->bIsSizing = true;
		break;
	case WM_SIZE:
		if (g_pApplication && wParam != SIZE_MINIMIZED)
		{
			int32 width = LOWORD(lParam);
			int32 height = HIWORD(lParam);

			if (g_pApplication->bIsSizing)
			{
				// 드래그 중: 스왑체인 리사이즈 X, 레터/필러박스 뷰포트만 적용
				g_pApplication->WindowWidth = width;
				g_pApplication->WindowHeight = height;
				URenderer::GetInstance().UseAspectFitViewport(width, height);
			}
			else
			{
				// 평소 리사이즈: 실제 리사이즈 + 풀윈도우 뷰포트
				URenderer::GetInstance().ResizeBuffers(width, height);
				URenderer::GetInstance().UseFullWindowViewport();
				g_pApplication->OnResize(width, height); // 카메라 갱신
			}
		}
		break;
	case WM_EXITSIZEMOVE:
		if (g_pApplication)
		{
			g_pApplication->bIsSizing = false;
			int32 width = g_pApplication->WindowWidth;
			int32 height = g_pApplication->WindowHeight;
			if (width > 0 && height > 0)
			{
				URenderer::GetInstance().ResizeBuffers(width, height);
				URenderer::GetInstance().UseFullWindowViewport();
				g_pApplication->OnResize(width, height);
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}



	return 0;
}