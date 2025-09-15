#include "stdafx.h"
#include "UCharInfoManager.h"
#include "Inc/DDSTextureLoader.h"

#pragma comment(lib, "DirectXTK.lib")

UCharInfoManager::UCharInfoManager()
{
}

UCharInfoManager::~UCharInfoManager()
{
}

void UCharInfoManager::Init()
{
	HRESULT hr = DirectX::CreateDDSTextureFromFile
	(	
		gDevice,
		gContext,
		L"DejaVu Sans Mono.dds",
		nullptr,
		&gTexture
	);

	if (FAILED(hr))
	{
		MessageBox(hWnd, L"텍스처 로드 실패", L"Error", MB_OK);
	}

	float cellSize = 1.0f / 16.0f; // 0.0625

	for (int ascii = 0; ascii < 256; ++ascii)
	{
		int row = ascii / 16;
		int col = ascii % 16;

		CharacterInfo info
		{
			col* cellSize;
			row * cellSize;
			cellSize;
			cellSize;
		}

		CharInfoMap[(char)ascii] = info;
		UE_LOG("%c", &ascii); // Debug용
	}
}

CharacterInfo UCharInfoManager::GetUVInfoByChar(char c)
{
	auto charInfoIterator = CharInfoMap.find(c);

	if (charInfoIterator != CharInfoMap.end())
	{
		return charInfoIterator->second;
	}
	
	return CharacterInfo(0.0f, 0.0f, 0.0f, 0.0f);
}

bool UCharInfoManager::IsValidInfo(CharacterInfo& CharInfo)
{
	if (CharInfo.height == 0.0f || CharInfo.width == 0.0f)
	{
		return false;
	}

	return true;
}