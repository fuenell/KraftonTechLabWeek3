#include "stdafx.h"
#include "USubUVManager.h"

USubUVManager& USubUVManager::GetInstance()
{
	static USubUVManager Instance;
	return Instance;
}

bool USubUVManager::Initialize(
	const wchar_t* FilePath,
	uint32 CellNumInRow,
	uint32 CellLifeSpawn
)
{
	ID3D11Device* Device = URenderer::GetInstance().GetDevice();
	ID3D11DeviceContext* DeviceContext = URenderer::GetInstance().GetDeviceContext();

	// Vertex, Pixel Shader, Input Layout 생성
	ID3DBlob* VSBlob = URenderer::CompileShader(
		L"ShaderSubUV.hlsl",
		"VS_Main",
		"vs_5_0"
	);
	if (!VSBlob)
		return false;

	VertexShader = URenderer::CreateVertexShader(Device, VSBlob);

	ID3DBlob* PSBlob = URenderer::CompileShader(
		L"ShaderSubUV.hlsl",
		"PS_Main",
		"ps_5_0"
	);
	if (!PSBlob)
		return false;

	PixelShader = URenderer::CreatePixelShader(Device, PSBlob);

	D3D11_INPUT_ELEMENT_DESC Layout[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
	  D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	InputLayout = URenderer::CreateInputLayout(
		Device,
		Layout,
		ARRAYSIZE(Layout),
		VSBlob
	);

	VSBlob->Release();
	PSBlob->Release();

	// 샘플러 및 셰이더 리소스 뷰 생성

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = sampDesc.AddressV = sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	Device->CreateSamplerState(&sampDesc, &SamplerState);

	ShaderResourceView = URenderer::CreateDDSTextureFromFile(
		Device,
		DeviceContext,
		FilePath
	);

	// vertex, index, constant 버퍼 생성

	TArray<FVertexPosUV> VertexArray = {
		// 첫 번째 삼각형 (CCW: 좌하 -> 우하 -> 좌상
		// )
		{ 0, -0.5f, -0.5f, 0.0f, 1.0f}, // 좌하
		{ 0,  0.5f, -0.5f, 0.0f, 0.0f}, // 좌상
		{  0, -0.5f, 0.5f, 1.0f, 1.0f}, // 우하
		{  0,  0.5f, 0.5f, 1.0f, 0.0f}  // 우상
	};

	TArray<uint32> IndexArray = { 0, 1, 2, 1, 3, 2 };

	VertexStride = sizeof(FVertexPosUV);
	IndexStride = sizeof(uint32);

	VertexBufferSize = VertexStride * VertexArray.size();
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
		return false;

	Desc.ByteWidth = IndexBufferSize;
	Desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	IndexBuffer = URenderer::CreateBuffer(Device, Desc, (const void*)IndexArray.data());
	if (!IndexBuffer)
		return false;

	ConstantBuffer = URenderer::CreateConstantBuffer(Device);
	if (!ConstantBuffer)
		return false;

	this->CellLifeSpawn = CellLifeSpawn;
	this->CellNumInRow = CellNumInRow;

	return true;
}

bool USubUVManager::UpdateConstantBuffer(
	FVector ModelScale,
	FMatrix CameraRotation,
	FMatrix View,
	FMatrix Projection
)
{
	if (!ConstantBuffer)
		return false;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	HRESULT Hr = DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	if (FAILED(Hr))
	{
		OutputDebugString(L"Update ConstantBuffer failed.\n");
		return false;
	}

	// ConstantBuffer에 들어갈 데이터를 계산
	CBTransform CBT = {};

	// SubUV의 MVP 행렬을 계산한다.
	FMatrix S = FMatrix::Scale(ModelScale.X, ModelScale.Y, ModelScale.Z);
	// 항상 카메라를 마주 보도록 한다.
	FMatrix R = FMatrix::Inverse(CameraRotation);
	FMatrix T = FMatrix::TranslationRow(ModelTranslation.X, ModelTranslation.Y, ModelTranslation.Z);

	FMatrix ModelTransform = S * R * T;
	FMatrix MVP = ModelTransform * View * Projection;

	URenderer::CopyRowMajor(CBT.MVP, MVP);

	// 현재 몇번째 셀을 가리키는지 정의한다.
	// CellNumInRow를 통해 텍스처에 셀이 몇 개 존재하는지 정의한다.
	// CellLifeSpawn을 통해 셀이 몇 프레임 동안 지속되는지 정의한다. 
	CBT.CellNumInRow = CellNumInRow;
	// 오버플로우 방지
	static uint32 CurrentCellIndex;
	// 하나의 셀은 CellLifeSpawn 변수만큼의 프레임 동안 지속된다.
	CurrentCellIndex %= (CellNumInRow * CellNumInRow * CellLifeSpawn);
	CBT.CurrentCellIndex = CurrentCellIndex / CellLifeSpawn;
	CurrentCellIndex++;

	// 계산한 데이터를 constant buffer에 전달한다.
	memcpy(MappedResource.pData, &CBT, sizeof(CBTransform));
	DeviceContext->Unmap(ConstantBuffer, 0);

	return true;
}

void USubUVManager::Bind()
{
	// IA
	DeviceContext->IASetInputLayout(InputLayout);
	UINT Offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexStride, &Offset);
	DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, Offset);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// VS
	DeviceContext->VSSetShader(VertexShader, nullptr, 0);
	DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);

	// PS
	DeviceContext->PSSetShader(PixelShader, nullptr, 0);
	DeviceContext->PSSetShaderResources(0, 1, &ShaderResourceView);
	DeviceContext->PSSetSamplers(0, 1, &SamplerState);
}

void USubUVManager::Render()
{
	DeviceContext->DrawIndexed(6, 0, 0);
}

void USubUVManager::Release()
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

	if (SamplerState)
	{
		SamplerState->Release();
		SamplerState = nullptr;
	}

	if (ShaderResourceView)
	{
		ShaderResourceView->Release();
		ShaderResourceView = nullptr;
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

	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
		ConstantBuffer = nullptr;
	}
}