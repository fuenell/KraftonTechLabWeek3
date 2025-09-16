#pragma once

#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

class UTextureManager : UEngineSubsystem
{
public:
	/** 전역 인스턴스 접근 */
	static UTextureManager& GetInstance()
	{
		static UTextureManager Instance; // 스택(static storage) 싱글톤
		return Instance;
	}
	

public:
	bool Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void Release();



public:
	TMap < std::string, ID3D11ShaderResourceView*>& GetSRVMap() { return SRVMap; }

private:
	UTextureManager() {};
	~UTextureManager()
	{
		Release();
	};

	UTextureManager(const UTextureManager&);
	UTextureManager& operator=(const UTextureManager&);

private:
	bool RegisterSRVWithDDS(const std::string nameKey, const wchar_t* filePath, HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	TMap<std::string, ID3D11ShaderResourceView*> SRVMap;
};

