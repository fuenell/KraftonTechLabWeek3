#include "stdafx.h"
#include "USpriteManager.h"
#include "UTextureManager.h"

USpriteManager::USpriteManager() :
	VertexShader(nullptr),
	PixelShader(nullptr),
	InputLayout(nullptr),
	VertexBuffer(nullptr),
	VertexBufferSize(0),
	IndexBufferSize(0),
	VertexStride(0),
	IndexStride(0)
{
}

bool USpriteManager::Initialize(ID3D11Device* Device, uint32 InDefaultMaxChars)
{
	// === 1. Vertex Shader ===
	ID3DBlob* vsBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "VSMain", "vs_5_0");
	VertexShader = URenderer::CreateVertexShader(Device, vsBlob);

	// === 2. Pixel Shader ===
	ID3DBlob* psBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "PSMain", "ps_5_0");
	PixelShader = URenderer::CreatePixelShader(Device, psBlob);

	// === 3. Input Layout ===
	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &InputLayout);

	vsBlob->Release();
	psBlob->Release();

	// === 4. SamplerState ===
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Device->CreateSamplerState(&sampDesc, &SamplerState);

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

	HRESULT hr = Device->CreateBuffer(&VBDesc, nullptr, &VertexBuffer);
	if (FAILED(hr) || !VertexBuffer) { return false; }

	D3D11_BUFFER_DESC IBDesc = {};
	IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	IBDesc.ByteWidth = IndexCapacityBytes;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = Device->CreateBuffer(&IBDesc, nullptr, &IndexBuffer);
	if (FAILED(hr) || !IndexBuffer) { return false; }

	CurrentIndexCount = 0;


	return true;
}

// 버텍스 버퍼와 인덱스 버퍼를 생성
void USpriteManager::SetBuffer(ID3D11Device* Device)
{
	VertexStride = sizeof(FVertexPosColor4);
	VertexBufferSize = VertexStride * VertexArray.size();

	IndexStride = sizeof(uint32);
	IndexBufferSize = IndexStride * IndexArray.size();

	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.ByteWidth = VertexBufferSize;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	VertexBuffer = URenderer::CreateBuffer(Device, Desc, (const void *)VertexArray.data());
	if (!VertexBuffer)
	{

	}

	Desc.ByteWidth = IndexBufferSize;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	IndexBuffer = URenderer::CreateBuffer(Device, Desc, (const void*)IndexArray.data());
	if (!IndexBuffer)
	{

	}
	
}

void USpriteManager::SetBufferUV(ID3D11Device* Device)
{
	VertexStride = sizeof(FVertexPosUV);
	VertexBufferSize = VertexStride * VertexArray.size();

	IndexStride = sizeof(uint32);
	IndexBufferSize = IndexStride * IndexArray.size();

	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.ByteWidth = VertexBufferSize;
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	VertexBuffer = URenderer::CreateBuffer(Device, Desc, (const void*)VertexArray.data());
	if (!VertexBuffer)
		;

	Desc.ByteWidth = IndexBufferSize;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	IndexBuffer = URenderer::CreateBuffer(Device, Desc, (const void*)IndexArray.data());
	if (!IndexBuffer)
		;
}

void USpriteManager::BeginFrame()
{
	VertexArray.clear();
	IndexArray.clear();
}

void USpriteManager::Bind(ID3D11DeviceContext* DeviceContext)
{
	// Set shaders
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	DeviceContext->IASetInputLayout(InputLayout);

	// Set primitive topology (default to triangle list)
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	auto& SRVMap = UTextureManager::GetInstance().GetSRVMap();
	auto It = SRVMap.find("DejaVu");
	if (It != SRVMap.end() && It->second)
	{
		ID3D11ShaderResourceView* SRV = It->second;
		DeviceContext->PSSetShaderResources(0, 1, &SRV);
	}

	DeviceContext->PSSetSamplers(0, 1, &SamplerState);
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

bool USpriteManager::SetUUIDVertices(ID3D11Device* Device, float AspectRatio, uint32 UUID, float RenderSize, FBounds WorldBound, FMatrix Modeling, FMatrix View, FMatrix Projection)
{
	// 모델을 둘러싸고 있는 Bounding Box의 8개의 점 중 가장 높이가 높은 점을 찾는다.
	FString UUIDString = FString("UUID : ") + std::to_string(UUID);

	FVector4 BoundEdges[8];
	BoundEdges[0] = { WorldBound.Min.X, WorldBound.Min.Y, WorldBound.Min.Z, 1.0f };
	BoundEdges[1] = { WorldBound.Max.X, WorldBound.Min.Y, WorldBound.Min.Z, 1.0f };
	BoundEdges[2] = { WorldBound.Min.X, WorldBound.Max.Y, WorldBound.Min.Z, 1.0f };
	BoundEdges[3] = { WorldBound.Max.X, WorldBound.Max.Y, WorldBound.Min.Z, 1.0f };
	BoundEdges[4] = { WorldBound.Min.X, WorldBound.Min.Y, WorldBound.Max.Z, 1.0f };
	BoundEdges[5] = { WorldBound.Max.X, WorldBound.Min.Y, WorldBound.Max.Z, 1.0f };
	BoundEdges[6] = { WorldBound.Min.X, WorldBound.Max.Y, WorldBound.Max.Z, 1.0f };
	BoundEdges[7] = { WorldBound.Max.X, WorldBound.Max.Y, WorldBound.Max.Z, 1.0f };

	for (int i = 0, s = ARRAYSIZE(BoundEdges); i < s; i++)
	{
		BoundEdges[i] = (View * Projection).TransformVectorRow(BoundEdges[i]);
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

	RenderCenter = (Modeling * View).TransformVectorRow(RenderCenter);

	if (RenderCenter.Z < 0.0f)
		return false;
	
	RenderCenter = Projection.TransformVectorRow(RenderCenter);
	RenderCenter = RenderCenter / RenderCenter.W;

	// Bounding Box 중 NDC 범위 안 유효한 점이 있다면
	if (!(Highest.Y == std::numeric_limits<float>::lowest()))
		RenderCenter.Y = Highest.Y;

	uint64 StringLen = UUIDString.size();
	float StartPosX = RenderCenter.X - ((float)StringLen * RenderSize * 0.5f * (1 / AspectRatio));

	for (uint64 i = 0, s = StringLen; i < s; i++)
	{
		CharacterInfo charInfo = Atlas.GetCharInfo(UUIDString[i]);

		float u = charInfo.u;
		float v = charInfo.v;
		float width = charInfo.width;
		float height = charInfo.height;

		FVertexPosUV Vertex1 = { StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, u, v + height };
		FVertexPosUV Vertex2 = { StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, u + width, v + height };
		FVertexPosUV Vertex3 = { StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, 0.0f, u, v };
		FVertexPosUV Vertex4 = { StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, 0.0f, u + width, v };

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
