#pragma once

#include "UEngineStatics.h"

struct CharacterInfo
{
    CharacterInfo()
    {

    }
    CharacterInfo(float InU, float InV, float InWidth, float InHeight)
    {
        U = InU;
        V = InV;
        Width = InWidth;
        Height = InHeight;
    }

    float U = 0.0f;
    float V = 0.0f;
    float Width = 0.0f;
    float Height = 0.0f;
};

class FontAtlas
{
public:
    FontAtlas();
    ~FontAtlas();

public:
    bool Initialize();
    CharacterInfo GetCharInfo(char c);

private:
    void InitCharInfoMap();

private:
    TMap<char, CharacterInfo> CharInfoMap;
};

