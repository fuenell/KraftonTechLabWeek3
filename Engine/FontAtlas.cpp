#include "stdafx.h"
#include "FontAtlas.h"

FontAtlas::FontAtlas()
{
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
    float CellSize = 1.0f / 16.0f; // 0.0625

    for (int Ascii = 0; Ascii < 256; ++Ascii)
    {
        int Row = Ascii / 16;
        int Col = Ascii % 16;

        CharacterInfo info
        {
            Col * CellSize,
            Row * CellSize,
            CellSize,
            CellSize
        };
        CharInfoMap[(char)Ascii] = info;
    }
}

CharacterInfo FontAtlas::GetCharInfo(char c)
{
    auto CharInfoIt = CharInfoMap.find(c);

    if (CharInfoIt != CharInfoMap.end())
    {
        CharacterInfo CharInfo = CharInfoIt->second;

        return CharInfo;
    }
    return CharacterInfo(0.0f, 0.0f, 0.0f, 0.0f);
}


bool IsValidCharInfo(CharacterInfo& InCharInfo)
{
    if (InCharInfo.Width == 0 || InCharInfo.Height == 0)
    {
        return false;
    }
    return true;
}

void FontAtlas::Release()
{
    CharInfoMap.clear();
}