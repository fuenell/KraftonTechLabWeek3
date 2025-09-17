#pragma once

#include "URenderer.h"
#include "FVertexPosUV.h"
#include "UTextureManager.h"
#include "FontAtlas.h"

class USpriteManager
{
public:
	static USpriteManager& GetInstance()
	{
		static USpriteManager Instance; // 스택(static storage) 싱글톤
		return Instance;
	}

private:
	USpriteManager();
	~USpriteManager();

	USpriteManager(const USpriteManager&);
	USpriteManager& operator=(const USpriteManager&);

public:
	// BillBoard를 그리기 위해 필요한 자원들을 생성
	bool Initialize(ID3D11Device* InDevice, uint32 InDefaultMaxChars = 128);

	// 파이프라인 설정
	void BeginFrame();
	void Bind(ID3D11DeviceContext* InDeviceContext);
	void Render(ID3D11DeviceContext* InDeviceContext);
	void Release();

public:
	bool SetUUIDVertices(ID3D11Device* InDevice, float InAspectRatio, uint32 InUUID, float InRenderSize, float InModelScale, FMatrix InModeling, FMatrix InView, FMatrix InProjection);

private:
	FontAtlas				Atlas;

	TArray<FVertexPosUV>	VertexArray;
	TArray<uint32>			IndexArray;

	ID3D11VertexShader*		VertexShader{};
	ID3D11PixelShader*		PixelShader{};
	ID3D11InputLayout*		InputLayout{};
	ID3D11Buffer*			VertexBuffer{};
	ID3D11Buffer*			IndexBuffer{};
	ID3D11SamplerState*		SamplerState{};

	UINT					VertexBufferSize{};
	UINT					IndexBufferSize{};
	UINT					VertexStride{};
	UINT					IndexStride{};

	uint32					VertexCapacityBytes{};
	uint32					IndexCapacityBytes{};
	uint32					CurrentIndexCount{}; 
};