#include "stdafx.h"
#include "UTextureManager.h"
#include "Inc/DDSTextureLoader.h"

#pragma comment(lib, "Lib/DirectXTK.lib")

UTextureManager::UTextureManager()
{
}

UTextureManager::~UTextureManager()
{
}

bool UTextureManager::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	bool bSuccess = true;
	bSuccess |= RegisterSRVWithDDS("DejaVu San Mono", L"DDS/DejaVu Sans Mono.dds", hWnd, device, deviceContext);

	return bSuccess;
}

bool UTextureManager::RegisterSRVWithDDS(const std::string nameKey, const wchar_t* filePath, HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* gTexture = nullptr;

	HRESULT hr = DirectX::CreateDDSTextureFromFile
	(
		device,
		deviceContext,
		filePath,
		nullptr,
		&gTexture
	);

	if (FAILED(hr))
	{
		std::wstring errMsg = filePath;

		errMsg += L" 텍스처 로드 실패";

		MessageBox(hWnd, errMsg.c_str(), L"Error", MB_OK);

		return false;
	}

	SRVMap[nameKey] = gTexture;

	return true;
}

void UTextureManager::Release()
{
	for (auto& pair : SRVMap)
	{
		if (pair.second)  // nullptr 체크
		{
			pair.second->Release(); // ID3D11* 계열 COM 객체는 delete가 아니라 Release하기
			pair.second = nullptr;
		}
	}

	SRVMap.clear();
}