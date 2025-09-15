#include "stdafx.h"
#include "SpriteRenderer.h"

bool SpriteRenderer::Initialize(ID3D11Device* Device)
{
	return SpriteManager.Initialize(Device);
}

void SpriteRenderer::Bind(ID3D11DeviceContext* DeviceContext)
{
	SpriteManager.Bind(DeviceContext);
}

void SpriteRenderer::Render(ID3D11DeviceContext* DeviceContext)
{
	SpriteManager.Render(DeviceContext);
}

void SpriteRenderer::Release()
{
	SpriteManager.Release();
}