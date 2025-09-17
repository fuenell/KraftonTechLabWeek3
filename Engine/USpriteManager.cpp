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

bool USpriteManager::SetUUIDVertices(ID3D11Device* InDevice, float InAspectRatio, uint32 InUUID, float InRenderSize, float InModelScale, FMatrix InModeling, FMatrix InView, FMatrix InProjection)
{
	FString UUIDString = FString("UUID : ") + std::to_string(InUUID);

	FVector4 ObjectCenter = { 0.0f, 0.0f, 0.0f, 1.0f };

	ObjectCenter = (InModeling * InView).TransformVectorRow(ObjectCenter);

	if (ObjectCenter.Z < 0.0f)
	{
		return false;
	}
		
	ObjectCenter = InProjection.TransformVectorRow(ObjectCenter);

	FVector4 RenderCenter = ObjectCenter / ObjectCenter.W;

	RenderCenter.Y -= 0.2f;

	uint64 StringLen = UUIDString.size();
	float StartPosX = RenderCenter.X - ((float)StringLen * InRenderSize * 0.5f * (1 / InAspectRatio));


	for (uint64 i = 0; i < StringLen; i++)
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
}
