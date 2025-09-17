#include "stdafx.h"
#include "FontAtlas.h"

FontAtlas::FontAtlas()
{
    Initialize();
}

FontAtlas::~FontAtlas()
{
}

bool FontAtlas::Initialize()
{
    InitCharInfoMap();

    return true;
}

void FontAtlas::InitCharInfoMap()
{
    float cellSize = 1.0f / 16.0f; // 0.0625

    for (int ascii = 0; ascii < 256; ++ascii)
    {
        int row = ascii / 16;
        int col = ascii % 16;

        CharacterInfo info
        {
            col * cellSize,
            row * cellSize,
            cellSize,
            cellSize
        };

        charInfoMap[(char)ascii] = info;
    }
}

CharacterInfo FontAtlas::GetCharInfo(char c)
{
    auto charInfoIt = charInfoMap.find(c);

    if (charInfoIt != charInfoMap.end())
    {
        CharacterInfo charInfo = charInfoIt->second;
        
        return charInfo;
    }

    return CharacterInfo(0.0f, 0.0f, 0.0f, 0.0f);
}

bool IsValidCharInfo(CharacterInfo& charInfo)
{
    if (charInfo.width == 0 || charInfo.height == 0)
    {
        return false;
    }

    return true;
}

void FontAtlas::Release()
{
    charInfoMap.clear();
}