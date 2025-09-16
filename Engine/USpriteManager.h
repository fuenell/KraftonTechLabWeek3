#pragma once

#include "URenderer.h"
#include "FVertexPosUV.h"
#include "UTextureManager.h"
#include "FontAtlas.h"

class USpriteManager
{
public:
	USpriteManager();

	// BillBoard를 그리기 위해 필요한 자원들을 생성
	bool Initialize(ID3D11Device* Device, uint32 InDefaultMaxChars = 128);
	// 버퍼 생성
	void SetBuffer(ID3D11Device* Device);
	void SetBufferUV(ID3D11Device* Device);

	// 파이프라인 설정
	void BeginFrame();
	void Bind(ID3D11DeviceContext* DeviceContext);
	void Render(ID3D11DeviceContext* DeviceContext);
	void Release();


	bool SetUUIDVertices(ID3D11Device* Device, float AspectRatio, uint32 UUID, float RenderSize, float ModelScale, FMatrix Modeling, FMatrix View, FMatrix Projection);

private:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	ID3D11SamplerState* SamplerState;

	UINT VertexBufferSize;
	UINT IndexBufferSize;
	UINT VertexStride;
	UINT IndexStride;

	FontAtlas Atlas;
	TArray<FVertexPosUV> VertexArray;
	TArray<uint32> IndexArray;

	uint32 VertexCapacityBytes = 0;
	uint32 IndexCapacityBytes = 0;
	uint32 CurrentIndexCount = 0; // DrawIndexed용
};