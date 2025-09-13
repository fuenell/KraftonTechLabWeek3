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
	: hWnd(nullptr)
	, bIsRunning(false)
	, bIsInitialized(false)
	, windowTitle(DefaultTitle)
	, windowWidth(1024)
	, windowHeight(768)
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

bool Application::Initialize(HINSTANCE hInstance, const std::wstring& title, int32 width, int32 height)
{
	if (bIsInitialized)
		return false;

	UClass::ResolveTypeBitsets();

	windowTitle = title;
	windowWidth = width;
	windowHeight = height;

	// Create main window
	if (!CreateMainWindow(hInstance))
	{
		return false;
	}

	// Initialize core systems
	if (!timeManager.Initialize(60))
	{
		MessageBox(hWnd, L"Failed to initialize TimeManager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!renderer.Initialize(hWnd))
	{
		MessageBox(hWnd, L"Failed to create D3D11 device and swap chain", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!renderer.CreateShader())
	{
		MessageBox(hWnd, L"Failed to create shaders", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!renderer.CreateConstantBuffer())
	{
		MessageBox(hWnd, L"Failed to create constant buffer", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!meshManager.Initialize(&renderer))
	{
		MessageBox(hWnd, L"Failed to initialize mesh manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!sceneManager.Initialize(g_pApplication))
	{
		MessageBox(hWnd, L"Failed to initialize scene manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!raycastManager.Initialize(&renderer, &inputManager))
	{
		MessageBox(hWnd, L"Failed to initialize raycast manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}
	if (!gui.Initialize(hWnd, renderer.GetDevice(), renderer.GetDeviceContext()))
	{
		return false;
	}

	// 여기서 일단은 그리드 렌더링에 필요한 vb, ib ,cb, vs, ps, ia 설정
	// 추후 수정예정 why? => 아직은 그리드만 구현했기 때문 
	if (!LineBatcherManager.Initialize(renderer.GetDevice(), 1024))
	{
		MessageBox(hWnd, L"Failed to initialize raycast manager", L"Engine Error", MB_OK | MB_ICONERROR);
		return false;
	}


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
		timeManager.BeginFrame();

		


		// 대강 이런식으로 추가 구현 예정
		/*
		for (auto& bb : BBoxes)                                     // 3) 디버그 라인/박스 등
			LineBatcherManager.AddBoundingBox(bb, bbWorld);
		for (auto& dl : DebugLines)
			LineBatcherManager.AddLine(dl.a, dl.b, dl.color);
		*/


		inputManager.Update();
		ProcessMessages();

		if (!bIsRunning)
			break;

		InternalUpdate();
		InternalRender();

		timeManager.EndFrame();
		timeManager.WaitForTargetFrameTime();
	}
}

void Application::Shutdown()
{
	if (!bIsInitialized)
		return;

	bIsRunning = false;

	// Allow derived classes to cleanup
	OnShutdown();

	// Shutdown core systems
	gui.Shutdown();
	renderer.ReleaseConstantBuffer();
	renderer.ReleaseShader();
	renderer.Release();

	bIsInitialized = false;
}

bool Application::CreateMainWindow(HINSTANCE hInstance)
{
	// Register window class
	WNDCLASSW wndclass = {};
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = WindowClass;
	wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClassW(&wndclass))
	{
		return false;
	}

	// Calculate window size including borders
	RECT windowRect = { 0, 0, windowWidth, windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int32 adjustedWidth = windowRect.right - windowRect.left;
	int32 adjustedHeight = windowRect.bottom - windowRect.top;

	// Create window
	hWnd = CreateWindowExW(
		0,
		WindowClass,
		windowTitle.c_str(),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		adjustedWidth, adjustedHeight,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd)
	{
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return true;
}

void Application::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			bIsRunning = false;
			return;
		}
	}
}

void Application::InternalUpdate()
{
	float deltaTime = static_cast<float>(timeManager.GetDeltaTime());


	// Call derived class update
	Update(deltaTime);
}

void Application::InternalRender()
{
	// Prepare rendering
	renderer.Prepare();
	renderer.PrepareShader();

	// Call derived class render
	Render();


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Batch Rendering
	// 여기서 라인들 초기화
	LineBatcherManager.BeginFrame();

	// 2) 그리드 쌓기 (원하는 색/간격/개수)
	const int gridCount = 100;
	const uint32_t colMain = 0x40AAAAAA; // ABGR
	const uint32_t colAxis = 0x80FFFFFF;
	LineBatcherManager.AddGrid(LineBatcherManager.GridSpacing, gridCount, colMain, colAxis);

	ID3D11DeviceContext* ctx = renderer.GetDeviceContext();
	const FMatrix view = sceneManager.GetScene()->GetCamera()->GetView();   // 네 쪽의 뷰 행렬 getter
	const FMatrix proj = sceneManager.GetScene()->GetCamera()->GetProj();   // 네 쪽의 프로젝션 행렬 getter
	LineBatcherManager.Render(ctx, view, proj);
	///////////////////////////////////////////////////////////////////////////////////////////////

	// Render GUI
	gui.BeginFrame();
	gui.Render();
	RenderGUI();
	gui.EndFrame();

	// Present the frame
	renderer.SwapBuffer();
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
		g_pApplication->inputManager.ProcessMessage(hWnd, message, wParam, lParam);
	}

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_ENTERSIZEMOVE:
		if (g_pApplication) g_pApplication->isSizing = true;
		break;
	case WM_SIZE:
		if (g_pApplication && wParam != SIZE_MINIMIZED)
		{
			int32 width = LOWORD(lParam);
			int32 height = HIWORD(lParam);

			if (g_pApplication->isSizing)
			{
				// 드래그 중: 스왑체인 리사이즈 X, 레터/필러박스 뷰포트만 적용
				g_pApplication->windowWidth = width;
				g_pApplication->windowHeight = height;
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
			g_pApplication->isSizing = false;
			int32 width = g_pApplication->windowWidth;
			int32 height = g_pApplication->windowHeight;
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

bool Application::OnInitialize()
{
	return true;

}

UScene* Application::CreateDefaultScene()
{
	return new UScene();
}
