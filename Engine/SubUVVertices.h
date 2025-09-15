﻿#pragma once
#include "stdafx.h"
#include "FVertexPosColor.h"
#include "TArray.h"

// CW(시계방향) 기준 XY 평면 정사각형
// 현재 UMesh가 FVertexPosColor에 대해서만 작동하게 짜여있는 관계로
// FVertexPosColor에서 Color의 r g 부분을 Texcoord의 UV로 취급하여 사용합니다.
// b a 부분은 버립니다.

inline TArray<FVertexPosColor> subuv_vertices = {
	// 첫 번째 삼각형 (CW: 좌상 -> 우상 -> 좌하)
	{ -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // 좌상 - Red
	{  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}, // 우상 - Green
	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}, // 좌하 - Blue

	// 두 번째 삼각형 (CW: 우상 -> 우하 -> 좌하)
	{  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}, // 우상 - Green
	{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}, // 우하 - Yellow
	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}  // 좌하 - Blue
};