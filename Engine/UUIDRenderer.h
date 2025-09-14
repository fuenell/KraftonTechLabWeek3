#pragma once

#include "stdafx.h"
#include "BillBoardManager.h"

class UUIDRenderer
{
private:
public:
	// BillBoard를 그리기 위해 필요한 자원들을 생성
	bool Initialize(ID3D11Device* Device);
	// 버퍼 생성
	// BillBoard가 카메라 뒤에 있으면 false 반환
	bool SetUUIDVertices(
		ID3D11Device* Device,
		float AspectRatio,
		uint32 UUID,
		float RenderSize,
		float ModelScale,
		FMatrix Modeling,
		FMatrix View,
		FMatrix Projection
	);
	// 파이프라인 설정
	void Bind(ID3D11DeviceContext* DeviceContext);
	void Render(ID3D11DeviceContext* DeviceContext);
	void Release();
private:
	BillBoardManager BBManager;
};