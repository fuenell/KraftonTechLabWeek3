#include "stdafx.h"
#include "SpriteRenderer.h"

bool USpriteRenderer::Initialize(ID3D11Device* Device, UTextureManager* InTextureManager)
{
	return SpriteManager.Initialize(Device, InTextureManager);
}

void USpriteRenderer::Bind(ID3D11DeviceContext* DeviceContext)
{
	SpriteManager.Bind(DeviceContext);
}

void USpriteRenderer::Render(ID3D11DeviceContext* DeviceContext)
{
	SpriteManager.Render(DeviceContext);
}

void USpriteRenderer::Release()
{
	SpriteManager.Release();
}