#pragma once

#include "stdafx.h"
#include "SpriteManager.h"

class SpriteRenderer
{
public:
	// Sprite를 그리기 위해 필요한 자원들을 생성
	bool Initialize(ID3D11Device* Device);
	// 파이프라인 설정
	void Bind(ID3D11DeviceContext* DeviceContext);
	// 그리기 및 버퍼 해제
	void Render(ID3D11DeviceContext* DeviceContext);
	// 자원 해제
	void Release();
	SpriteManager SpriteManager;
};