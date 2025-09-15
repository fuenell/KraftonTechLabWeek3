#pragma once

#include "UEngineStatics.h"

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

class FontAtlas
{
public:
    FontAtlas();
    virtual ~FontAtlas();

    void InitCharInfoMap();
    CharacterInfo GetCharInfo(char c);
    static bool IsValidCharInfo(CharacterInfo& charInfo);

private:
    TMap<char, CharacterInfo> charInfoMap;
};

