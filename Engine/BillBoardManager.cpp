#include "stdafx.h"
#include "BillBoardManager.h"

BillBoardManager::BillBoardManager() :
	VertexShader(nullptr),
	PixelShader(nullptr),
	InputLayout(nullptr),
	VertexBuffer(nullptr)
{
}

bool BillBoardManager::Initialize(ID3D11Device* Device)
{

	// Load vertex shader from file
	ID3DBlob* VSBlob = URenderer::CompileShader(L"BillBoard.vs", "main", "vs_5_0");
	if (!VSBlob)
		return false;

	// Create vertex shader
	VertexShader = URenderer::CreateVertexShader(Device, VSBlob);
	if (!VertexShader)
		return false;

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	InputLayout = URenderer::CreateInputLayout(
		Device,
		inputElements,
		ARRAYSIZE(inputElements),
		VSBlob
	);
	if (!InputLayout)
		return false;

	ID3DBlob* PSBlob = URenderer::CompileShader(L"BillBoard.ps", "main", "ps_5_0");
	if (!PSBlob)
		return false;

	PixelShader = URenderer::CreatePixelShader(Device, PSBlob);
	if (!PixelShader)
		return false;

	TArray<FVertexPosColor4> VA = {
		{0.3f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
	};

	SetBuffer(Device, VA);

	return true;
}

void BillBoardManager::SetBuffer(ID3D11Device* Device, TArray<FVertexPosColor4> VertexArray)
{
	D3D11_BUFFER_DESC Desc = {};
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.ByteWidth = sizeof(FVertexPosColor4) * VertexArray.size();
	Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;
	Desc.StructureByteStride = 0;

	VertexBuffer = URenderer::CreateVertexBuffer(Device, Desc, (const void *)VertexArray.data());
	if (!VertexBuffer)
		;
}

void BillBoardManager::Bind(ID3D11DeviceContext* DeviceContext)
{
	// Set shaders
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);

	// Set input layout
	DeviceContext->IASetInputLayout(InputLayout);
	UINT Stride = sizeof(FVertexPosColor4);
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);

	// Set primitive topology (default to triangle list)
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BillBoardManager::Render(ID3D11DeviceContext* DeviceContext)
{
	DeviceContext->Draw(3, 0);
}

void BillBoardManager::Release()
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
}