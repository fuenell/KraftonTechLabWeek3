#pragma once

#include "URenderer.h"

class BillBoardManager
{
private:
public:
	BillBoardManager();

	// BillBoard를 그리기 위해 필요한 자원들을 생성
	bool Initialize(ID3D11Device* Device);
	// 버퍼 생성
	void SetBuffer(
		ID3D11Device* Device,
		TArray<FVertexPosColor4> VertexArray,
		TArray<uint32> IndexArray);
	// 파이프라인 설정
	void Bind(ID3D11DeviceContext* DeviceContext);
	void Render(ID3D11DeviceContext* DeviceContext);
	void Release();
private:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	UINT VertexBufferSize;
	UINT IndexBufferSize;
	UINT VertexStride;
	UINT IndexStride;
};