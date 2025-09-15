#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "UTextMesh.h"
#include "Matrix.h"
#include "UEngineSubsystem.h"

// URenderer.h or cpp 상단
struct CBTransform
{
	// HLSL의 row_major float4x4와 메모리 호환을 위해 float[16]로 보냄
	float MVP[16];
	float MeshColor[4];
	float IsSelected;
	float Padding[3];
};

class URenderer : UEngineSubsystem
{
	DECLARE_UCLASS(URenderer, UEngineSubsystem)

public:
	URenderer();
	~URenderer();

	// Initialization and cleanup
	bool Initialize(HWND WindowHandle);
	bool CreateShader();
	bool CreateRasterizerState();
	bool CreateRasterizerStateWire();
	bool CreateBlendState();
	bool CreateConstantBuffer();
	void Release();
	void ReleaseShader();
	void ReleaseConstantBuffer();

	// Buffer creation
	bool ReleaseVertexBuffer(ID3D11Buffer* Buffer);
	bool ReleaseIndexBuffer(ID3D11Buffer* Buffer);

	// Texture creation
	ID3D11Texture2D* CreateTexture2D(int32 Width, int32 Height, DXGI_FORMAT Format,
		const void* Data = nullptr);
	ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* Texture);
	bool ReleaseTexture(ID3D11Texture2D* Texture);
	bool ReleaseShaderResourceView(ID3D11ShaderResourceView* Srv);

	// Rendering operations
	void Prepare();
	void PrepareShader();
	void SwapBuffer();
	void Clear(float R = 0.0f, float G = 0.0f, float B = 0.0f, float A = 1.0f);

	// Drawing operations
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation = 0, INT BaseVertexLocation = 0);
	void Draw(UINT VertexCount, UINT StartVertexLocation = 0);
	void DrawMesh(UMesh* Mesh);
	void DrawTextMesh(UTextMesh* Tmesh);
	void DrawMeshOnTop(UMesh* Mesh);

	// Resource binding
	void SetVertexBuffer(ID3D11Buffer* Buffer, UINT Stride, UINT Offset = 0);
	void SetIndexBuffer(ID3D11Buffer* Buffer, DXGI_FORMAT Format = DXGI_FORMAT_R32_UINT);
	void SetConstantBuffer(ID3D11Buffer* Buffer, UINT Slot = 0);
	void SetTexture(ID3D11ShaderResourceView* Srv, UINT Slot = 0);
	void SetRasterizerState(EViewModeIndex InViewModeIndex);

	// Constant buffer updates
	bool UpdateConstantBuffer(const void* Data, size_t SizeInBytes);

	// Window resize handling
	bool ResizeBuffers(int32 Width, int32 Height);

	// Setter

	// Getters
	ID3D11Device* GetDevice() const { return Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }
	IDXGISwapChain* GetSwapChain() const { return SwapChain; }
	EViewModeIndex GetViewModeIndex() const { return ViewModeIndex; }
	bool IsInitialized() const { return bIsInitialized; }

	// Utility functions
	bool CheckDeviceState();
	void GetBackBufferSize(int32& Width, int32& Height);

	void SetViewProj(const FMatrix& V, const FMatrix& P); // 내부에 VP 캐시
	void SetModel(const FMatrix& M, const FVector4& Color, bool IsSelected);               // M*VP → b0 업로드
	void SetTargetAspect(float A) { if (A > 0.f) TargetAspect = A; }
	// targetAspect를 내부에서 사용 (카메라에 의존 X)
	D3D11_VIEWPORT MakeAspectFitViewport(int32 WinW, int32 WinH) const;
	// 드래그 중 호출: currentViewport만 갈아끼움
	void UseAspectFitViewport(int32 WinW, int32 WinH)
	{
		CurrentViewport = MakeAspectFitViewport(WinW, WinH);
	}
	// 평소엔 풀 윈도우
	void UseFullWindowViewport()
	{
		CurrentViewport = Viewport;
	}

	// Error handling
	static void LogError(const char* Function, HRESULT Hr);
	static bool CheckResult(HRESULT Hr, const char* Function);

	static ID3DBlob* CompileShader(LPCWSTR PFileName, LPCSTR PEntrypoint, LPCSTR PTarget);

	static ID3D11VertexShader* CreateVertexShader(ID3D11Device* InDevice, ID3DBlob* VSBlob);

	static ID3D11InputLayout* CreateInputLayout(
		ID3D11Device* InDevice,
		D3D11_INPUT_ELEMENT_DESC* InputElements,
		int64 InputElementsSize,
		ID3DBlob* VSBlob);

	static ID3D11PixelShader* CreatePixelShader(ID3D11Device* InDevice, ID3DBlob* PSBlob);

	static ID3D11Buffer* CreateBuffer(
		ID3D11Device* InDevice,
		D3D11_BUFFER_DESC BufferDesc,
		const void* Data
	);

private:
	// Internal helper functions
	bool CreateDeviceAndSwapChain(HWND WindowHandle);
	bool CreateRenderTargetView();
	bool CreateDepthStencilView(int32 Width, int32 Height);
	bool SetupViewport(int32 Width, int32 Height);

	// 행렬 복사 핼퍼
	static inline void CopyRowMajor(float Dst[16], const FMatrix& Src)
	{
		for (int32 r = 0; r < 4; ++r)
			for (int32 c = 0; c < 4; ++c)
				Dst[r * 4 + c] = Src.M[r][c];
	}

private:
	// Core D3D11 objects
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	IDXGISwapChain* SwapChain;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	ID3D11RasterizerState* RasterizerState;
	ID3D11RasterizerState* RasterizerStateWire;

	// Shader objects
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;
	ID3D11SamplerState* SamplerState;

	// Constant buffer
	ID3D11Buffer* ConstantBuffer;

	// blendstate buffer
	ID3D11BlendState* BlendState;

	// Viewport
	D3D11_VIEWPORT Viewport;
	D3D11_VIEWPORT CurrentViewport; // 실제로 사용할 뷰포트(레터박스/필러박스 포함)
	float TargetAspect = 16.0f / 9.0f;

	// Window handle
	HWND HWnd;

	// Render state
	bool bIsInitialized;

	FMatrix MVP;                 // 프레임 캐시
	CBTransform MCBData;

	EViewModeIndex ViewModeIndex = EViewModeIndex::VMI_Normal;
};