#pragma once

#include "UEngineStatics.h"
#include "UEngineSubsystem.h"

struct CharacterInfo
{
	float u;
	float v;
	float width;
	float height;

	CharacterInfo(float U, float V, float Width, float Height)
	{
		u = U;
		v = V;
		width = Width;
		height = Height;
	}
};

class UCharInfoManager : UEngineSubsystem
{
public:
	UCharInfoManager();
	~UCharInfoManager();

	void Init();
	CharacterInfo GetUVInfoByChar(char c);
	bool IsValidInfo(CharacterInfo& CharInfo);

private:
	TMap<char, CharacterInfo> CharInfoMap;
};