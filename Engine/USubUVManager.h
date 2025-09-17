#pragma once
#include "stdafx.h"
#include "URenderer.h"
#include "FVertexPosUV.h"

class USubUVManager
{
public:
	struct CBTransform
	{
		float MVP[16];
		uint32 CellNumInRow;
		uint32 CurrentCellIndex;
	};

public:
	static USubUVManager& GetInstance();

public:
	bool Initialize(const wchar_t* InFilePath,uint32 InCellNumInRow,uint32 InCellLifeSpawn);
	bool UpdateConstantBuffer(FVector InModelScale,FMatrix InCameraRotation,FMatrix InView,FMatrix InProjection);
	void Bind();
	void Render();
	void Release();

public:
	void TriggerAt(const FVector& InWorldPos, float InNowSeconds);
	void Deactivate()							  { bActive = false; }
	bool  IsActive()						const { return bActive; }

	//getter
	float GetDurationSeconds()				const { return DurationSeconds; }
	float GetLastTriggerTime()				const { return LastTriggerTimeSeconds; }
	float GetCurrendTime()					const { return LastTriggerTimeSeconds; }
	const FVector& GetModelTranslation()	const { return ModelTranslation; }


	//setter
	void  SetDurationSeconds(float S)						{ DurationSeconds = S; }
	void SetModelTranslation(FVector InModelTranslation)	{ ModelTranslation = InModelTranslation; }
	void SetCurrentTime(float InCurrentTime)				{ LastTriggerTimeSeconds = InCurrentTime; }

private:
	USubUVManager() = default;
	~USubUVManager() = default;


private:
	FVector						ModelTranslation{};

	ID3D11Device*				Device{};
	ID3D11DeviceContext*		DeviceContext{};

	ID3D11VertexShader*			VertexShader{};
	ID3D11PixelShader*			PixelShader{};
	ID3D11InputLayout*			InputLayout{};
	
	ID3D11SamplerState*			SamplerState{};
	ID3D11ShaderResourceView*	ShaderResourceView{};

	ID3D11Buffer*				VertexBuffer{};
	ID3D11Buffer*				IndexBuffer{};
	ID3D11Buffer*				ConstantBuffer{};

	UINT						VertexStride{};
	UINT						IndexStride{};
	UINT						VertexBufferSize{};
	UINT						IndexBufferSize{};

	uint32						CellNumInRow{};
	uint32						CellLifeSpawn{};

	float						LastTriggerTimeSeconds = -1.0f;
	float						DurationSeconds = 2.4f;
	bool						bActive = false;
};