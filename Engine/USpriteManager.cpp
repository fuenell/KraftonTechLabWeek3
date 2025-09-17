#include "stdafx.h"
#include "USpriteManager.h"
#include "UTextureManager.h"

USpriteManager::USpriteManager()
{
}

USpriteManager::~USpriteManager()
{
}

bool USpriteManager::Initialize(ID3D11Device* InDevice, uint32 InDefaultMaxChars)
{
	// === 1. Vertex Shader ===
	ID3DBlob* vsBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "VSMain", "vs_5_0");
	VertexShader = URenderer::CreateVertexShader(InDevice, vsBlob);

	// === 2. Pixel Shader ===
	ID3DBlob* psBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "PSMain", "ps_5_0");
	PixelShader = URenderer::CreatePixelShader(InDevice, psBlob);

	// === 3. Input Layout ===
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	InDevice->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &InputLayout);

	vsBlob->Release();
	psBlob->Release();

	// === 4. SamplerState ===
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	InDevice->CreateSamplerState(&sampDesc, &SamplerState);

	// 어디를 읽을지를 정하는 지도
	Atlas.Initialize();

	// ===== VB/IB 생성 (Dynamic + CPU_WRITE) =====
	VertexStride = sizeof(FVertexPosUV);
	IndexStride = sizeof(uint32);

	VertexCapacityBytes = VertexStride * (InDefaultMaxChars * 4); // 4 verts/char
	IndexCapacityBytes = IndexStride * (InDefaultMaxChars * 6); // 6 idx/char

	D3D11_BUFFER_DESC VBDesc = {};
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.ByteWidth = VertexCapacityBytes;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = InDevice->CreateBuffer(&VBDesc, nullptr, &VertexBuffer);
	if (FAILED(hr) || !VertexBuffer) { return false; }

	D3D11_BUFFER_DESC IBDesc = {};
	IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	IBDesc.ByteWidth = IndexCapacityBytes;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = InDevice->CreateBuffer(&IBDesc, nullptr, &IndexBuffer);
	if (FAILED(hr) || !IndexBuffer) { return false; }

	CurrentIndexCount = 0;

	return true;
}

void USpriteManager::BeginFrame()
{
	VertexArray.clear();
	IndexArray.clear();
}

void USpriteManager::Bind(ID3D11DeviceContext* InDeviceContext)
{
	// Set shaders
	InDeviceContext->VSSetShader(VertexShader, nullptr, 0);
	InDeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	InDeviceContext->IASetInputLayout(InputLayout);

	// Set primitive topology (default to triangle list)
	InDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto& SRVMap = UTextureManager::GetInstance().GetSRVMap();
	//auto It = SRVMap.find("DejaVu");
	auto It = SRVMap.find("Roboto");
	if (It != SRVMap.end() && It->second)
	{
		ID3D11ShaderResourceView* SRV = It->second;
		InDeviceContext->PSSetShaderResources(0, 1, &SRV);
	}

	InDeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

void USpriteManager::Render(ID3D11DeviceContext* DeviceContext)
{
	D3D11_MAPPED_SUBRESOURCE Mapped;
	DeviceContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	memcpy(Mapped.pData, VertexArray.data(), sizeof(FVertexPosUV) * VertexArray.size());
	DeviceContext->Unmap(VertexBuffer, 0);

	// 2) IB 업로드
	DeviceContext->Map(IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
	memcpy(Mapped.pData, IndexArray.data(), sizeof(uint32_t) * IndexArray.size());
	DeviceContext->Unmap(IndexBuffer, 0);

	CurrentIndexCount = static_cast<uint32>(IndexArray.size());

	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexStride, &Offset);
	DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset);


	if (CurrentIndexCount == 0) return;
	DeviceContext->DrawIndexed(static_cast<UINT>(CurrentIndexCount), 0, 0);
}

void USpriteManager::Release()
{
	Atlas.Release();

	if (VertexShader)
	{
		VertexShader->Release();
		VertexShader = nullptr;
	}

	if (PixelShader)
	{
		PixelShader->Release();
		PixelShader = nullptr;
	}

	if (InputLayout)
	{
		InputLayout->Release();
		InputLayout = nullptr;
	}

	if (VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}

	if (IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}

	if (SamplerState)
	{
		SamplerState->Release();
		SamplerState = nullptr;
	}
}

bool USpriteManager::SetUUIDVertices(ID3D11Device* InDevice, float InAspectRatio, uint32 InUUID, float InRenderSize, FBounds InWorldBound, FMatrix InModeling, FMatrix InView, FMatrix InProjection)
{
	// 모델을 둘러싸고 있는 Bounding Box의 8개의 점 중 가장 높이가 높은 점을 찾는다.
	FString UUIDString = FString("UUID:") + std::to_string(InUUID);

	FVector4 BoundEdges[8];
	BoundEdges[0] = { InWorldBound.Min.X, InWorldBound.Min.Y, InWorldBound.Min.Z, 1.0f };
	BoundEdges[1] = { InWorldBound.Max.X, InWorldBound.Min.Y, InWorldBound.Min.Z, 1.0f };
	BoundEdges[2] = { InWorldBound.Min.X, InWorldBound.Max.Y, InWorldBound.Min.Z, 1.0f };
	BoundEdges[3] = { InWorldBound.Max.X, InWorldBound.Max.Y, InWorldBound.Min.Z, 1.0f };
	BoundEdges[4] = { InWorldBound.Min.X, InWorldBound.Min.Y, InWorldBound.Max.Z, 1.0f };
	BoundEdges[5] = { InWorldBound.Max.X, InWorldBound.Min.Y, InWorldBound.Max.Z, 1.0f };
	BoundEdges[6] = { InWorldBound.Min.X, InWorldBound.Max.Y, InWorldBound.Max.Z, 1.0f };
	BoundEdges[7] = { InWorldBound.Max.X, InWorldBound.Max.Y, InWorldBound.Max.Z, 1.0f };

	for (int i = 0, s = ARRAYSIZE(BoundEdges); i < s; i++)
	{
		BoundEdges[i] = (InView * InProjection).TransformVectorRow(BoundEdges[i]);
		BoundEdges[i] = BoundEdges[i] / BoundEdges[i].W;
	}

	FVector4 Highest = {0.0f, std::numeric_limits<float>::lowest(), 0.0f, 1.0f};
    
	for (int i = 0, s = ARRAYSIZE(BoundEdges); i < s; i++)
	{
		// NDC 범위를 벗어나면 투영에서 제외
		if (
			BoundEdges[i].X < -1.0f || BoundEdges[i].X > 1.0f ||
			BoundEdges[i].Y < -1.0f || BoundEdges[i].Y > 1.0f ||
			BoundEdges[i].Z < 0.0f || BoundEdges[i].Z > 1.0f
			)
			continue;
		if (BoundEdges[i].Y > Highest.Y)
			Highest = BoundEdges[i];
	}


	// UUID를 렌더할 NDC의 기준점 위치
	FVector4 RenderCenter = { 0.0f, 0.0f, 0.0f, 1.0f };

	RenderCenter = (InModeling * InView).TransformVectorRow(RenderCenter);

	if (RenderCenter.Z < 0.0f)
		return false;
	
	RenderCenter = InProjection.TransformVectorRow(RenderCenter);
	RenderCenter = RenderCenter / RenderCenter.W;

	// Bounding Box 중 NDC 범위 안 유효한 점이 있다면
	if (!(Highest.Y == std::numeric_limits<float>::lowest()))
		RenderCenter.Y = Highest.Y;

	uint64 StringLen = UUIDString.size();
	float StartPosX = RenderCenter.X - ((float)StringLen * InRenderSize * 0.5f * (1 / InAspectRatio));

	for (uint64 i = 0, s = StringLen; i < s; i++)
	{
		CharacterInfo CharInfo = Atlas.GetCharInfo(UUIDString[i]);

		float U = CharInfo.U;
		float V = CharInfo.V;
		float Width = CharInfo.Width;
		float Height = CharInfo.Height;

		FVertexPosUV Vertex1 = { StartPosX + i * InRenderSize * (1 / InAspectRatio), RenderCenter.Y, 0.0f, U, V + Height };
		FVertexPosUV Vertex2 = { StartPosX + (i + 1) * InRenderSize * (1 / InAspectRatio), RenderCenter.Y, 0.0f, U + Width, V + Height };
		FVertexPosUV Vertex3 = { StartPosX + i * InRenderSize * (1 / InAspectRatio), RenderCenter.Y + (float)InRenderSize, 0.0f, U, V };
		FVertexPosUV Vertex4 = { StartPosX + (i + 1) * InRenderSize * (1 / InAspectRatio), RenderCenter.Y + (float)InRenderSize, 0.0f, U + Width, V };

		VertexArray.push_back(Vertex1);
		VertexArray.push_back(Vertex2);
		VertexArray.push_back(Vertex3);
		VertexArray.push_back(Vertex4);

		IndexArray.push_back(0 + 4 * i);
		IndexArray.push_back(1 + 4 * i);
		IndexArray.push_back(2 + 4 * i);
		IndexArray.push_back(2 + 4 * i);
		IndexArray.push_back(1 + 4 * i);
		IndexArray.push_back(3 + 4 * i);
	}

	return true;
}
