#include "stdafx.h"
#include "UTextureManager.h"
#include "Inc/DDSTextureLoader.h"

#pragma comment(lib, "Lib/DirectXTK.lib")


bool UTextureManager::Initialize(HWND hWnd, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
{
	bool bSuccess = true;
	bSuccess |= RegisterSRVWithDDS("DejaVu", L"DDS/DejaVu Sans Mono.dds", hWnd, InDevice, InDeviceContext);
	bSuccess |= RegisterSRVWithDDS("Roboto", L"DDS/Roboto-Bold.dds", hWnd, InDevice, InDeviceContext);

	return bSuccess;
}

bool UTextureManager::RegisterSRVWithDDS(const std::string InNameKey, const wchar_t* InFilePath, HWND hWnd, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
{
	ID3D11ShaderResourceView* gTexture = nullptr;

	HRESULT hr = DirectX::CreateDDSTextureFromFile (InDevice, InDeviceContext, InFilePath, nullptr, &gTexture);

	if (FAILED(hr))
	{
		std::wstring ErrMsg = InFilePath;

		ErrMsg += L" 텍스처 로드 실패";

		MessageBox(hWnd, ErrMsg.c_str(), L"Error", MB_OK);

		return false;
	}
	SRVMap[InNameKey] = gTexture;

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