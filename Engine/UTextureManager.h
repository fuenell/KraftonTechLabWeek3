#pragma once

#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

class UTextureManager : UEngineSubsystem
{
public:
	UTextureManager();
	~UTextureManager();

	bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool RegisterSRVWithDDS(const std::string nameKey, const wchar_t* filePath, HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Release();

	TMap<std::string, ID3D11ShaderResourceView*> SRVMap;
};

