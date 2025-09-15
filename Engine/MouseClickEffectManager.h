#pragma once

#include "SpriteRenderer.h"

class MouseClickEffectManager : public SpriteRenderer
{
private:
public:
<<<<<<< Updated upstream
	// 버퍼 생성
	void SetSubUVVertices(uint32 MouseXPos, uint32 MouseYPos, )
	{

	}
=======
	void SaveSubUVSpawnRequest(
		uint32 MouseXPos,
		uint32 MouseYPos,
		uint32 ScreenWidth,
		uint32 ScreenHeight
	)
	{
		float NdcX = ((float)MouseXPos / (float)ScreenWidth) * 2.0f - 1.0f;
		float NdcY = 1.0f - ((float)MouseYPos / (float)ScreenHeight) * 2.0f;
	}

	// 버퍼 생성
	void CalculateSubUVSpawnPosition(
	)
	{
		float NdcX = ((float)MouseXPos / (float)ScreenWidth) * 2.0f - 1.0f;
		float NdcY = 1.0f - ((float)MouseYPos / (float)ScreenHeight) * 2.0f;
	}
private:
	struct SubUVSpawnRequest
	{
		float NdcX;
		float NdcY;
		int Timer;

	};

	TArray<FVertexPosColor4> VertexArray;
	TArray<uint32> IndexArray;
>>>>>>> Stashed changes
};