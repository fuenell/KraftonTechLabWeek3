#pragma once

// Windows 관련 (가장 먼저)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// C++ 표준 라이브러리
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <cassert>
#include <cmath>
#include <optional>
#include <filesystem>
#include <climits>
#include <bitset>
#include <stack>
#include <queue>

// DirectX 관련
#include <d3d11.h>
#include <d3dcompiler.h>

//#include <DirectXMath.h>

// ImGui 관련 (DirectX 이후에)
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImguiConsole.h"

// --- 엔진 핵심 헤더 (Engine Core Headers) ---
// 이 헤더들은 변경 빈도가 낮고, 의존성이 높아야 합니다.
#include "MemoryTracker.h"
#include "UClass.h"
#include "RTTI.h"			// RTTI 시스템
#include "UObject.h"		// 최상위 오브젝트

// 수학 관련 헤더
#include "Vector.h"
#include "Random.h"

// 라이브러리 링킹
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// Engine forward declarations
class Application;
class URenderer;
class UInputManager;
class UGUI;
class UTimeManager;

// Common macros
#define SAFE_RELEASE(p) if(p) { p->Release(); p = nullptr; }
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr; }
#define SAFE_DELETE_ARRAY(p) if(p) { delete[] p; p = nullptr; }

const float PI = 3.14159265358979323846f;
const float PIDIV4 = PI / 4.0f;   // XM_PIDIV4 대체

const float DegreeToRadian = PI / 180.0f;

// Engine namespace (optional)
namespace Engine
{
	// Common engine types and utilities can go here
}

struct FBounds
{
	FVector Min;
	FVector Max;
};

enum class PrimitiveType
{
	Sphere,
	Cube,
	Plane,
	SpotLight,
	End
};

enum class EViewModeIndex : uint32
{
	VMI_Normal,
	VMI_Lit,
	VMI_Unlit,
	VMI_Wireframe,
};
