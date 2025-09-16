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
	FVector ModelTranslation{};


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

	float   LastTriggerTimeSeconds = -1.0f;
	float   DurationSeconds = 0.144f; // 144 ms (원하면 바꿔)
	bool    bActive = false;

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
		FVector ModelScale,
		FMatrix CameraRotation,
		FMatrix View,
		FMatrix Projection
	);


	void TriggerAt(const FVector& InWorldPos, float InNowSeconds)
	{
		ModelTranslation = InWorldPos;
		LastTriggerTimeSeconds = InNowSeconds;
		bActive = true;
	}

	void Deactivate() { bActive = false; }
	bool  IsActive()               const { return bActive; }
	float GetLastTriggerTime()     const { return LastTriggerTimeSeconds; }
	void  SetDurationSeconds(float S) { DurationSeconds = S; }
	float GetDurationSeconds()     const { return DurationSeconds; }


	void SetModelTranslation(FVector InModelTranslation) { ModelTranslation = InModelTranslation; }
	const FVector& GetModelTranslation() const { return ModelTranslation; }

	void SetCurrentTime(float InCurrentTime) { LastTriggerTimeSeconds = InCurrentTime; }
	float GetCurrendTime()const { return LastTriggerTimeSeconds; }

	void Bind();
	void Render();
	void Release();
};