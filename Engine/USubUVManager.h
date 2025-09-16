#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "FVertexPosUV.h"

class USubUVManager
{
private:
	struct CBTransform
	{
		float MVP[16];
		uint32 CellNumInRow;
		uint32 CurrentCellIndex;
	};
private:
	ID3D11Device* Device = URenderer::GetInstance().GetDevice();
	ID3D11DeviceContext* DeviceContext = URenderer::GetInstance().GetDeviceContext();

	ID3D11VertexShader* VertexShader = nullptr;
	ID3D11PixelShader* PixelShader = nullptr;
	ID3D11InputLayout* InputLayout = nullptr;
	
	ID3D11SamplerState* SamplerState = nullptr;
	ID3D11ShaderResourceView* ShaderResourceView = nullptr;

	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
	ID3D11Buffer* ConstantBuffer = nullptr;

	UINT VertexStride = 0;
	UINT IndexStride = 0;
	UINT VertexBufferSize = 0;
	UINT IndexBufferSize = 0;

	uint32 CellNumInRow = 0;
	uint32 CellLifeSpawn = 0;
private:
	USubUVManager() = default;
	~USubUVManager() = default;
public:
	static USubUVManager& GetInstance();

	bool Initialize(
		const wchar_t* FilePath,
		uint32 CellNumInRow,
		uint32 CellLifeSpawn
	);

	bool UpdateConstantBuffer(
		FVector ModelTranslation,
		FVector ModelScale,
		FMatrix CameraRotation,
		FMatrix View,
		FMatrix Projection
	);

	void Bind();
	void Render();
	void Release();
};