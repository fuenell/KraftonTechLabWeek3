#include "stdafx.h"
#include "USpriteManager.h"

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

bool USpriteManager::Initialize(ID3D11Device* Device, UTextureManager* InTextureManager)
{

	// Load vertex shader from file
	//ID3DBlob* VSBlob = URenderer::CompileShader(L"BillBoard.vs", "main", "vs_5_0");
	//if (!VSBlob)
	//	return false;
	//
	//// Create vertex shader
	//VertexShader = URenderer::CreateVertexShader(Device, VSBlob);
	//if (!VertexShader)
	//	return false;
	//
	//// Create input layout
	//D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};
	//
	//InputLayout = URenderer::CreateInputLayout(
	//	Device,
	//	inputElements,
	//	ARRAYSIZE(inputElements),
	//	VSBlob
	//);
	//if (!InputLayout)
	//	return false;
	//
	//ID3DBlob* PSBlob = URenderer::CompileShader(L"BillBoard.ps", "main", "ps_5_0");
	//if (!PSBlob)
	//	return false;
	//
	//PixelShader = URenderer::CreatePixelShader(Device, PSBlob);
	//if (!PixelShader)
	//	return false;



	ID3DBlob* vsBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "VSMain", "vs_5_0");

	VertexShader = URenderer::CreateVertexShader(Device, vsBlob);

	ID3DBlob* psBlob = URenderer::CompileShader(L"ShaderFont.hlsl", "PSMain", "ps_5_0");

	PixelShader = URenderer::CreatePixelShader(Device, psBlob);

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	Device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(),
		vsBlob->GetBufferSize(), &InputLayout);

	vsBlob->Release();
	psBlob->Release();

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Device->CreateSamplerState(&sampDesc, &SamplerState);

	TextureManager = InTextureManager;

	return true;
}


void USpriteManager::SetBuffer(
	ID3D11Device* Device,
	TArray<FVertexPosColor4> VertexArray,
	TArray<uint32> IndexArray)
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
		;

	Desc.ByteWidth = IndexBufferSize;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	IndexBuffer = URenderer::CreateBuffer(Device, Desc, (const void*)IndexArray.data());
	if (!IndexBuffer)
		;
}

void USpriteManager::SetBufferUV(ID3D11Device* Device, TArray<FVertexPosUV> VertexArray, TArray<uint32> IndexArray)
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

void USpriteManager::Bind(ID3D11DeviceContext* DeviceContext)
{
	// Set shaders
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	DeviceContext->IASetInputLayout(InputLayout);
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexStride, &Offset);
	DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset);

	// Set primitive topology (default to triangle list)
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* SRV = TextureManager->SRVMap["Roboto"];

	DeviceContext->PSSetShaderResources(0, 1, &SRV);
	DeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

void USpriteManager::Render(ID3D11DeviceContext* DeviceContext)
{
	DeviceContext->DrawIndexed(IndexBufferSize, 0, 0);
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
}