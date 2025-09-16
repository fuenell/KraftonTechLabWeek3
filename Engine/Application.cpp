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

	// Initialize core systems
	if (!TimeManager.Initialize(60))
	{
		MessageBox(HWnd, L"Failed to initialize TimeManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!Renderer.Initialize(HWnd))
	{
		MessageBox(HWnd, L"Failed to create D3D11 device and swap chain", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!Renderer.CreateShader())
	{
		MessageBox(HWnd, L"Failed to create shaders", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!Renderer.CreateConstantBuffer())
	{
		MessageBox(HWnd, L"Failed to create constant buffer", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!MeshManager.Initialize(&Renderer))
	{
		MessageBox(HWnd, L"Failed to initialize mesh manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!SceneManager.Initialize(g_pApplication))
	{
		MessageBox(HWnd, L"Failed to initialize scene manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!RaycastManager.Initialize(&Renderer, &InputManager))
	{
		MessageBox(HWnd, L"Failed to initialize raycast manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!Gui.Initialize(HWnd, Renderer.GetDevice(), Renderer.GetDeviceContext()))
	{
		return false;
	}
	if (!UUIDRenderer.Initialize(Renderer.GetDevice(), &TextureManager))
	{
		return false;
	}
	// 여기서 일단은 그리드 렌더링에 필요한 vb, ib ,cb, vs, ps, ia 설정
	// 추후 수정예정 why? => 아직은 그리드만 구현했기 때문 
	if (!LineBatcherManager.Initialize(Renderer.GetDevice(), 1024))
	{
		MessageBox(HWnd, L"Failed to initialize LineBatcherManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!TextureManager.Initialize(HWnd, Renderer.GetDevice(), Renderer.GetDeviceContext()))
	{
		MessageBox(HWnd, L"Failed to initialize TextureManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}


	/*
	ConfigManager은 반드시 Camera와 LineBatcherManager 및 변수를 받아오는 객체들이
	초기화된 이후에 초기화해야 한다.
	*/
	ConfigManager& ConfigManager = ConfigManager::Instance();
	ConfigManager.RegisterConfigTargets(&LineBatcherManager);
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
		TimeManager.BeginFrame();




		// 대강 이런식으로 추가 구현 예정
		/*
		for (auto& bb : BBoxes)                                     // 3) 디버그 라인/박스 등
			LineBatcherManager.AddBoundingBox(bb, bbWorld);
		for (auto& dl : DebugLines)
			LineBatcherManager.AddLine(dl.a, dl.b, dl.color);
		*/


		InputManager.Update();
		ProcessMessages();

		if (!bIsRunning)
			break;

		InternalUpdate();
		InternalRender();

		TimeManager.EndFrame();
		TimeManager.WaitForTargetFrameTime();
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
	Gui.Shutdown();
	Renderer.ReleaseConstantBuffer();
	Renderer.ReleaseShader();
	Renderer.Release();

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
	float DeltaTime = static_cast<float>(TimeManager.GetDeltaTime());


	// Call derived class update
	Update(DeltaTime);
}

void Application::InternalRender()
{
	// Prepare rendering
	Renderer.Prepare();
	Renderer.PrepareShader();

	// Call derived class render
	Render();

	// Render GUI
	Gui.BeginFrame();
	Gui.Render();
	RenderGUI();
	Gui.EndFrame();

	// Present the frame
	Renderer.SwapBuffer();
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
		g_pApplication->InputManager.ProcessMessage(hWnd, message, wParam, lParam);
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
				g_pApplication->GetRenderer().UseAspectFitViewport(width, height);
			}
			else
			{
				// 평소 리사이즈: 실제 리사이즈 + 풀윈도우 뷰포트
				g_pApplication->GetRenderer().ResizeBuffers(width, height);
				g_pApplication->GetRenderer().UseFullWindowViewport();
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
				g_pApplication->GetRenderer().ResizeBuffers(width, height);
				g_pApplication->GetRenderer().UseFullWindowViewport();
				g_pApplication->OnResize(width, height);
			}
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}



	return 0;
}