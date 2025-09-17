#pragma once
#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

class UTextureManager : UEngineSubsystem
{
public:
	// 싱글톤
	static UTextureManager& GetInstance()
	{
		static UTextureManager Instance; 
		return Instance;
	}
	
private:
	UTextureManager() {};
	~UTextureManager()
	{
		Release();
	};

	UTextureManager(const UTextureManager&);
	UTextureManager& operator=(const UTextureManager&);

public:
	bool Initialize(HWND hWnd, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext);
	void Release();

public:
	TMap < std::string, ID3D11ShaderResourceView*>& GetSRVMap() { return SRVMap; }

private:
	bool RegisterSRVWithDDS(const std::string InNameKey, const wchar_t* InFilePath, HWND hWnd, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext);

private:
	TMap<std::string, ID3D11ShaderResourceView*> SRVMap;
};

