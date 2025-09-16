#include "stdafx.h"
#include "URenderer.h"

IMPLEMENT_UCLASS(URenderer, UEngineSubsystem)

URenderer::URenderer()
	: Device(nullptr)
	, DeviceContext(nullptr)
	, SwapChain(nullptr)
	, RenderTargetView(nullptr)
	, DepthStencilView(nullptr)
	, VertexShader(nullptr)
	, PixelShader(nullptr)
	, InputLayout(nullptr)
	, ConstantBuffer(nullptr)
	, RasterizerState(nullptr)
	, HWnd(nullptr)
	, bIsInitialized(false)
{
	ZeroMemory(&Viewport, sizeof(Viewport));
}

URenderer::~URenderer()
{
	Release();
}

bool URenderer::Initialize(HWND WindowHandle)
{
	if (bIsInitialized)
		return true;

	HWnd = WindowHandle;

	// Create device and swap chain
	if (!CreateDeviceAndSwapChain(WindowHandle))
	{
		LogError("CreateDeviceAndSwapChain", E_FAIL);
		return false;
	}

	// Create render target view
	if (!CreateRenderTargetView())
	{
		LogError("CreateRenderTargetView", E_FAIL);
		return false;
	}

	// Get back buffer size and create depth stencil view
	int32 Width, Height;
	GetBackBufferSize(Width, Height);

	if (!CreateDepthStencilView(Width, Height))
	{
		LogError("CreateDepthStencilView", E_FAIL);
		return false;
	}

	// Setup viewport
	if (!SetupViewport(Width, Height))
	{
		LogError("SetupViewport", E_FAIL);
		return false;
	}

	if (!CreateRasterizerState())
	{
		LogError("CreateRasterizerState", E_FAIL);
		return false;
	}

	if (!CreateRasterizerStateWire())
	{
		LogError("CreateRasterizerState", E_FAIL);
		return false;
	}

	if (!CreateBlendState())
	{
		LogError("CreateBlendState", E_FAIL);
		return false;
	}




	bIsInitialized = true;
	return true;
}

// URenderer.cpp의 CreateShader() 함수를 다음과 같이 수정

bool URenderer::CreateShader()
{
	// Load vertex shader from file
	ID3DBlob* VSBlob = CompileShader(L"ShaderW0.hlsl", "mainVS", "vs_5_0");
	if (!VSBlob)
		return false;

	// Create vertex shader
	VertexShader = CreateVertexShader(Device, VSBlob);
	if (!VertexShader)
		return false;

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC InputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	InputLayout = CreateInputLayout(Device, InputElements, ARRAYSIZE(InputElements),
		VSBlob);
	if (!InputLayout)
		return false;

	ID3DBlob* PSBlob = nullptr;
	PSBlob = CompileShader(L"ShaderW0.hlsl", "mainPS", "ps_5_0");
	if (!PSBlob)
		return false;

	PixelShader = CreatePixelShader(Device, PSBlob);
	if (!PixelShader)
		return false;

	return true;
}

bool URenderer::CreateBlendState()
{
	D3D11_BLEND_DESC Bd = {};
	auto& Rt = Bd.RenderTarget[0];
	Rt.BlendEnable = TRUE;
	Rt.SrcBlend = D3D11_BLEND_SRC_ALPHA;      // 스트레이트 알파
	Rt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;  // (프리멀티면 ONE / INV_SRC_ALPHA)
	Rt.BlendOp = D3D11_BLEND_OP_ADD;
	Rt.SrcBlendAlpha = D3D11_BLEND_ONE;
	Rt.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	Rt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	Rt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT Hr = Device->CreateBlendState(&Bd, &BlendState);
	return CheckResult(Hr, "CreateBlendState");
}

bool URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	RasterizerDesc.CullMode = D3D11_CULL_BACK;          // 뒷면 제거
	RasterizerDesc.FrontCounterClockwise = TRUE;
	RasterizerDesc.DepthBias = 0;
	RasterizerDesc.DepthBiasClamp = 0.0f;
	RasterizerDesc.SlopeScaledDepthBias = 0.0f;
	RasterizerDesc.DepthClipEnable = TRUE;
	RasterizerDesc.ScissorEnable = FALSE;
	RasterizerDesc.MultisampleEnable = FALSE;
	RasterizerDesc.AntialiasedLineEnable = FALSE;

	HRESULT Hr = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
	return CheckResult(Hr, "CreateRasterizerState");
}

bool URenderer::CreateRasterizerStateWire()
{
	D3D11_RASTERIZER_DESC RasterizerDesc = {};
	RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterizerDesc.CullMode = D3D11_CULL_NONE;          // 뒷면 제거
	RasterizerDesc.FrontCounterClockwise = TRUE;

	HRESULT Hr = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerStateWire);
	return CheckResult(Hr, "CreateRasterizerStateWire");
}

bool URenderer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.ByteWidth = sizeof(CBTransform);   // ← 변경
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT Hr = Device->CreateBuffer(&BufferDesc, nullptr, &ConstantBuffer);
	return CheckResult(Hr, "CreateConstantBuffer");
}

bool URenderer::UpdateConstantBuffer(const void* Data, size_t SizeInBytes)
{
	if (!ConstantBuffer || !Data)
		return false;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT Hr = DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	if (FAILED(Hr))
	{
		LogError("Map ConstantBuffer", Hr);
		return false;
	}

	memcpy(MappedResource.pData, Data, SizeInBytes);
	DeviceContext->Unmap(ConstantBuffer, 0);

	return true;
}

// URenderer.cpp의 나머지 함수들 (기존 코드에 추가)

void URenderer::Release()
{
	if (!bIsInitialized)
		return;

	ReleaseShader();
	ReleaseConstantBuffer();

	SAFE_RELEASE(RasterizerStateWire);
	SAFE_RELEASE(RasterizerState);
	SAFE_RELEASE(DepthStencilView);
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(SwapChain);
	SAFE_RELEASE(DeviceContext);
	SAFE_RELEASE(Device);

	bIsInitialized = false;
	HWnd = nullptr;
}

void URenderer::ReleaseShader()
{
	SAFE_RELEASE(InputLayout);
	SAFE_RELEASE(PixelShader);
	SAFE_RELEASE(VertexShader);
}

void URenderer::ReleaseConstantBuffer()
{
	SAFE_RELEASE(ConstantBuffer);
}

bool URenderer::ReleaseVertexBuffer(ID3D11Buffer* Buffer)
{
	if (Buffer)
	{
		Buffer->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseIndexBuffer(ID3D11Buffer* Buffer)
{
	if (Buffer)
	{
		Buffer->Release();
		return true;
	}
	return false;
}

ID3D11Texture2D* URenderer::CreateTexture2D(int32 Width, int32 Height, DXGI_FORMAT Format, const void* Data)
{
	if (!Device || Width <= 0 || Height <= 0)
		return nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc = {};
	TextureDesc.Width = static_cast<UINT>(Width);
	TextureDesc.Height = static_cast<UINT>(Height);
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = Format;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA* PInitData = nullptr;
	D3D11_SUBRESOURCE_DATA InitData = {};
	if (Data)
	{
		InitData.pSysMem = Data;
		InitData.SysMemPitch = Width * 4; // Assuming 4 bytes per pixel (RGBA)
		PInitData = &InitData;
	}

	ID3D11Texture2D* Texture = nullptr;
	HRESULT Hr = Device->CreateTexture2D(&TextureDesc, PInitData, &Texture);

	if (FAILED(Hr))
	{
		LogError("CreateTexture2D", Hr);
		return nullptr;
	}

	return Texture;
}

ID3D11ShaderResourceView* URenderer::CreateShaderResourceView(ID3D11Texture2D* Texture)
{
	if (!Device || !Texture)
		return nullptr;

	D3D11_SHADER_RESOURCE_VIEW_DESC SrvDesc = {};
	SrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA format
	SrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SrvDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* Srv = nullptr;
	HRESULT Hr = Device->CreateShaderResourceView(Texture, &SrvDesc, &Srv);

	if (FAILED(Hr))
	{
		LogError("CreateShaderResourceView", Hr);
		return nullptr;
	}

	return Srv;
}

bool URenderer::ReleaseTexture(ID3D11Texture2D* Texture)
{
	if (Texture)
	{
		Texture->Release();
		return true;
	}
	return false;
}

bool URenderer::ReleaseShaderResourceView(ID3D11ShaderResourceView* Srv)
{
	if (Srv)
	{
		Srv->Release();
		return true;
	}
	return false;
}

void URenderer::Prepare()
{
	if (!DeviceContext)
		return;

	float BlendFactor[4] = { 0, 0, 0, 0 };
	DeviceContext->OMSetBlendState(BlendState, BlendFactor, 0xffffffff);

	// Set render target and depth stencil view
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// Set viewport
	DeviceContext->RSSetViewports(1, &CurrentViewport);

	// Clear render target and depth stencil
	Clear();
}

void URenderer::PrepareShader()
{
	if (!DeviceContext)
		return;



	// Set shaders
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	DeviceContext->IASetInputLayout(InputLayout);

	// Set primitive topology (default to triangle list)
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set rasterizer state (와인딩 순서 적용)
	switch (ViewModeIndex)
	{
	case EViewModeIndex::VMI_Normal:
		DeviceContext->RSSetState(RasterizerState);

		break;

	case EViewModeIndex::VMI_Wireframe:
		DeviceContext->RSSetState(RasterizerStateWire);
		break;

	default:
		DeviceContext->RSSetState(RasterizerState);
		break;
	}


	// Set constant buffer
	if (ConstantBuffer)
	{
		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	}
}

void URenderer::SwapBuffer()
{
	if (SwapChain)
	{
		SwapChain->Present(1, 0); // VSync enabled
	}
}

void URenderer::Clear(float R, float G, float B, float A)
{
	if (!DeviceContext)
		return;

	float ClearColor[4] = { R, G, B, A };

	if (RenderTargetView)
	{
		DeviceContext->ClearRenderTargetView(RenderTargetView, ClearColor);
	}

	if (DepthStencilView)
	{
		DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}

void URenderer::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	if (DeviceContext)
	{
		DeviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	}
}

void URenderer::Draw(UINT VertexCount, UINT StartVertexLocation)
{
	if (DeviceContext)
	{
		DeviceContext->Draw(VertexCount, StartVertexLocation);
	}
}

void URenderer::DrawMesh(UMesh* Mesh)
{
	if (!IsInitialized()) return;

	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Mesh->Stride, &Offset);
	DeviceContext->IASetPrimitiveTopology(Mesh->PrimitiveType);

	if (Mesh->IndexBuffer && Mesh->NumIndices > 0)
	{
		// 인덱스 버퍼가 있으면 DrawIndexed 호출
		DeviceContext->IASetIndexBuffer(Mesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->DrawIndexed(Mesh->NumIndices, 0, 0);
	}
	else
	{
		// 인덱스 버퍼가 없으면 기존 방식대로 Draw 호출
		DeviceContext->Draw(Mesh->NumVertices, 0);
	}
}

void URenderer::DrawTextMesh(UTextMesh* Tmesh)
{
	if (!Tmesh || !Tmesh->IsInitialized())
		return;

	UINT Offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &Tmesh->VertexBuffer, &Tmesh->Stride, &Offset);
	DeviceContext->IASetPrimitiveTopology(Tmesh->PrimitiveType);

	DeviceContext->Draw(Tmesh->NumVertices, 0);
}

void URenderer::DrawMeshOnTop(UMesh* Mesh)
{
	if (!Mesh || !Mesh->IsInitialized())
		return;

	// Create a depth-stencil state with depth testing disabled
	D3D11_DEPTH_STENCIL_DESC DsDesc = {};
	DsDesc.DepthEnable = FALSE;  // disable depth testing
	DsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	DsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* PDSState = nullptr;
	HRESULT Hr = Device->CreateDepthStencilState(&DsDesc, &PDSState);
	if (FAILED(Hr))
	{
		LogError("CreateDepthStencilState (DrawMeshOnTop)", Hr);
		return;
	}

	// Backup current depth-stencil state
	ID3D11DepthStencilState* POldState = nullptr;
	UINT StencilRef = 0;
	DeviceContext->OMGetDepthStencilState(&POldState, &StencilRef);

	// Set new state (no depth test)
	DeviceContext->OMSetDepthStencilState(PDSState, 0);

	// Draw mesh
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &Mesh->VertexBuffer, &Mesh->Stride, &Offset);
	DeviceContext->IASetPrimitiveTopology(Mesh->PrimitiveType);
	DeviceContext->Draw(Mesh->NumVertices, 0);

	// Restore previous depth state
	DeviceContext->OMSetDepthStencilState(POldState, StencilRef);

	// Release local COM objects
	SAFE_RELEASE(POldState);
	SAFE_RELEASE(PDSState);
}

void URenderer::SetVertexBuffer(ID3D11Buffer* Buffer, UINT Stride, UINT Offset)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->IASetVertexBuffers(0, 1, &Buffer, &Stride, &Offset);
	}
}

void URenderer::SetIndexBuffer(ID3D11Buffer* Buffer, DXGI_FORMAT Format)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->IASetIndexBuffer(Buffer, Format, 0);
	}
}

void URenderer::SetConstantBuffer(ID3D11Buffer* Buffer, UINT Slot)
{
	if (DeviceContext && Buffer)
	{
		DeviceContext->VSSetConstantBuffers(Slot, 1, &Buffer);
	}
}

void URenderer::SetTexture(ID3D11ShaderResourceView* Srv, UINT Slot)
{
	if (DeviceContext && Srv)
	{
		DeviceContext->PSSetShaderResources(Slot, 1, &Srv);
	}
}

void URenderer::SetRasterizerState(EViewModeIndex InViewModeIndex)
{
	ViewModeIndex = InViewModeIndex;
}

bool URenderer::ResizeBuffers(int32 Width, int32 Height)
{
	if (!SwapChain || Width <= 0 || Height <= 0)
		return false;

	// Release render target view before resizing
	SAFE_RELEASE(RenderTargetView);
	SAFE_RELEASE(DepthStencilView);

	// Resize swap chain buffers
	HRESULT Hr = SwapChain->ResizeBuffers(0, static_cast<UINT>(Width), static_cast<UINT>(Height),
		DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(Hr))
	{
		LogError("ResizeBuffers", Hr);
		return false;
	}

	// Recreate render target view
	if (!CreateRenderTargetView())
	{
		return false;
	}

	// Recreate depth stencil view
	if (!CreateDepthStencilView(Width, Height))
	{
		return false;
	}

	// Update viewport
	return SetupViewport(Width, Height);
}

bool URenderer::CheckDeviceState()
{
	if (!Device)
		return false;

	HRESULT Hr = Device->GetDeviceRemovedReason();
	if (FAILED(Hr))
	{
		LogError("Device Lost", Hr);
		return false;
	}

	return true;
}

void URenderer::GetBackBufferSize(int32& Width, int32& Height)
{
	Width = Height = 0;

	if (!SwapChain)
		return;

	ID3D11Texture2D* BackBuffer = nullptr;
	HRESULT Hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	if (SUCCEEDED(Hr) && BackBuffer)
	{
		D3D11_TEXTURE2D_DESC Desc;
		BackBuffer->GetDesc(&Desc);
		Width = static_cast<int32>(Desc.Width);
		Height = static_cast<int32>(Desc.Height);
		BackBuffer->Release();
	}
}

// Private helper functions

bool URenderer::CreateDeviceAndSwapChain(HWND WindowHandle)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = 0; // Use window size
	SwapChainDesc.BufferDesc.Height = 0;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = WindowHandle;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT Hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                   // Use default adapter
		D3D_DRIVER_TYPE_HARDWARE,    // Hardware acceleration
		nullptr,                   // No software module
		0,                         // No flags
		nullptr,                   // Use default feature levels
		0,                         // Number of feature levels
		D3D11_SDK_VERSION,         // SDK version
		&SwapChainDesc,              // Swap chain description
		&SwapChain,                // Swap chain output
		&Device,                   // Device output
		&FeatureLevel,               // Feature level output
		&DeviceContext             // Device context output
	);

	return CheckResult(Hr, "D3D11CreateDeviceAndSwapChain");
}

bool URenderer::CreateRenderTargetView()
{
	ID3D11Texture2D* BackBuffer = nullptr;
	HRESULT Hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	if (FAILED(Hr))
	{
		LogError("GetBuffer", Hr);
		return false;
	}

	Hr = Device->CreateRenderTargetView(BackBuffer, nullptr, &RenderTargetView);
	BackBuffer->Release();

	return CheckResult(Hr, "CreateRenderTargetView");
}

bool URenderer::CreateDepthStencilView(int32 Width, int32 Height)
{
	D3D11_TEXTURE2D_DESC DepthStencilDesc = {};
	DepthStencilDesc.Width = static_cast<UINT>(Width);
	DepthStencilDesc.Height = static_cast<UINT>(Height);
	DepthStencilDesc.MipLevels = 1;
	DepthStencilDesc.ArraySize = 1;
	DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ID3D11Texture2D* DepthStencilBuffer = nullptr;
	HRESULT Hr = Device->CreateTexture2D(&DepthStencilDesc, nullptr, &DepthStencilBuffer);

	if (FAILED(Hr))
	{
		LogError("CreateTexture2D (DepthStencil)", Hr);
		return false;
	}

	Hr = Device->CreateDepthStencilView(DepthStencilBuffer, nullptr, &DepthStencilView);
	DepthStencilBuffer->Release();

	return CheckResult(Hr, "CreateDepthStencilView");
}

bool URenderer::SetupViewport(int32 Width, int32 Height)
{
	Viewport.Width = static_cast<FLOAT>(Width);
	Viewport.Height = static_cast<FLOAT>(Height);
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.TopLeftX = 0.0f;
	Viewport.TopLeftY = 0.0f;
	// 기본은 풀 윈도우
	CurrentViewport = Viewport;
	return true;
}

void URenderer::SetViewProj(const FMatrix& V, const FMatrix& P)
{
	// row-vector 규약이면 곱셈 순서는 V*P가 아니라, 최종적으로 v*M*V*P가 되도록
	// 프레임 캐시엔 VP = V * P 저장
	MVP = V * P;
	// 여기서는 상수버퍼 업로드 안 함 (오브젝트에서 M과 합쳐서 업로드)
}

void URenderer::SetModel(const FMatrix& M, const FVector4& Color, bool bIsSelected)
{
	// per-object: MVP = M * VP
	FMatrix InMVP = M * MVP;
	CopyRowMajor(MCBData.MVP, InMVP);
	memcpy(MCBData.MeshColor, &Color, sizeof(float) * 4);
	MCBData.IsSelected = bIsSelected ? 1.0f : 0.0f;
	UpdateConstantBuffer(&MCBData, sizeof(MCBData));
}

D3D11_VIEWPORT URenderer::MakeAspectFitViewport(int32 WinW, int32 WinH) const
{
	D3D11_VIEWPORT Vp{};
	Vp.MinDepth = 0.0f; Vp.MaxDepth = 1.0f;

	float Wa = (WinH > 0) ? (float)WinW / (float)WinH : TargetAspect;
	if (Wa > TargetAspect)
	{
		Vp.Height = (float)WinH;
		Vp.Width = Vp.Height * TargetAspect;
		Vp.TopLeftY = 0.0f;
		Vp.TopLeftX = 0.5f * (WinW - Vp.Width);
	}
	else
	{
		Vp.Width = (float)WinW;
		Vp.Height = Vp.Width / TargetAspect;
		Vp.TopLeftX = 0.0f;
		Vp.TopLeftY = 0.5f * (WinH - Vp.Height);
	}
	return Vp;
}

void URenderer::LogError(const char* Function, HRESULT Hr)
{
	char ErrorMsg[512];
	sprintf_s(ErrorMsg, "URenderer::%s failed with HRESULT: 0x%08X", Function, Hr);
	OutputDebugStringA(ErrorMsg);
}

bool URenderer::CheckResult(HRESULT Hr, const char* Function)
{
	if (FAILED(Hr))
	{
		LogError(Function, Hr);
		return false;
	}
	return true;
}

ID3DBlob* URenderer::CompileShader(
	LPCWSTR PFileName,
	LPCSTR PEntrypoint,
	LPCSTR PTarget)
{
	// Load vertex shader from file
	ID3DBlob* ShaderBlob;
	ID3DBlob* ErrorBlob = nullptr;

	HRESULT Hr = D3DCompileFromFile(
		PFileName,               // 파일 경로
		nullptr,                 // 매크로 정의
		nullptr,                 // Include 핸들러
		PEntrypoint,               // 진입점 함수명
		PTarget,               // 셰이더 모델
		0,                       // 컴파일 플래그
		0,                       // 효과 플래그
		&ShaderBlob,             // 컴파일된 셰이더
		&ErrorBlob               // 에러 메시지
	);

	if (FAILED(Hr))
	{
		if (ErrorBlob)
		{
			OutputDebugStringA("Vertex Shader Compile Error:\n");
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			SAFE_RELEASE(ErrorBlob);
		}
		else
		{
			OutputDebugStringA("Failed to load vertex shader file");
		}
		return nullptr;
	}
	return ShaderBlob;
}

ID3D11VertexShader* URenderer::CreateVertexShader(
	ID3D11Device* InDevice,
	ID3DBlob* VSBlob
)
{
	ID3D11VertexShader* VertexShader;

	HRESULT Hr = InDevice->CreateVertexShader(
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		nullptr,
		&VertexShader
	);

	if (!CheckResult(Hr, "CreateVertexShader"))
	{
		SAFE_RELEASE(VSBlob);
		return nullptr;
	}
	return VertexShader;
}

ID3D11InputLayout* URenderer::CreateInputLayout(
	ID3D11Device* InDevice,
	D3D11_INPUT_ELEMENT_DESC* InputElements,
	int64 InputElementsSize,
	ID3DBlob* VSBlob)
{
	ID3D11InputLayout* InputLayout;

	HRESULT Hr = InDevice->CreateInputLayout(
		InputElements,
		InputElementsSize,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		&InputLayout
	);

	SAFE_RELEASE(VSBlob);
	if (!CheckResult(Hr, "CreateInputLayout"))
	{
		return nullptr;
	}
	return InputLayout;
}

ID3D11PixelShader* URenderer::CreatePixelShader(ID3D11Device* InDevice, ID3DBlob* PSBlob)
{
	ID3D11PixelShader* PixelShader;

	HRESULT Hr = InDevice->CreatePixelShader(
		PSBlob->GetBufferPointer(),
		PSBlob->GetBufferSize(),
		nullptr,
		&PixelShader
	);

	if (!CheckResult(Hr, "CreatePixelShader"))
	{
		SAFE_RELEASE(PSBlob);
		return nullptr;
	}
	return PixelShader;
}

ID3D11Buffer* URenderer::CreateBuffer(
	ID3D11Device* InDevice,
	D3D11_BUFFER_DESC BufferDesc,
	const void* Data
)
{
	if (!InDevice || !Data)
		return nullptr;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Data;

	ID3D11Buffer* Buffer = nullptr;
	HRESULT Hr = InDevice->CreateBuffer(&BufferDesc, &InitData, &Buffer);

	if (FAILED(Hr))
	{
		LogError("CreateBuffer", Hr);
		return nullptr;
	}

	return Buffer;
}

ID3D11Buffer* URenderer::CreateConstantBuffer(
	ID3D11Device* InDevice
)
{
	if (!InDevice)
		return nullptr;

	D3D11_BUFFER_DESC Desc = {};
	Desc.ByteWidth = sizeof(CBTransform);   // ← 변경
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Buffer* Buffer = nullptr;
	HRESULT Hr = InDevice->CreateBuffer(&Desc, nullptr, &Buffer);

	if (FAILED(Hr))
	{
		LogError("CreateBuffer", Hr);
		return nullptr;
	}

	return Buffer;
}

ID3D11ShaderResourceView* URenderer::CreateDDSTextureFromFile(
	ID3D11Device* Device,
	ID3D11DeviceContext* DeviceContext,
	const wchar_t* FilePath
)
{
	ID3D11ShaderResourceView* TextureResourceView = nullptr;

	HRESULT Hr = DirectX::CreateDDSTextureFromFile
	(
		Device,
		DeviceContext,
		FilePath,
		nullptr,
		&TextureResourceView
	);

	if (FAILED(Hr))
	{
		LogError("CreateDDSTextureFromFile", Hr);
		return nullptr;
	}

	return TextureResourceView;
}