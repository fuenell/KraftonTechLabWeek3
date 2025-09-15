#pragma once

#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

class UTextureManager : UEngineSubsystem
{
public:
	UTextureManager();
	~UTextureManager();

	void RegisterDDSToSRV();

private:
	TMap<std::string, ID3D11ShaderResourceView*> SRVMap;
};

