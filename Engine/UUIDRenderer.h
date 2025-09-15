#pragma once

#include "stdafx.h"
#include "SpriteRenderer.h"
#include "FontAtlas.h"

class UUIDRenderer : public USpriteRenderer
{
public:
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

private:
	FontAtlas Atlas;
};