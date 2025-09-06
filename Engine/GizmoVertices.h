#pragma once
#include "FVertexSimple.h"

float gridSize = 10.0f; // 한쪽 방향으로의 격자 크기 (예: -10에서 +10까지)
float halfSize = gridSize;

FVertexSimpleOld gizmo_grid_vertices[] =
{
	// 세로 선 (Z축 방향)
	{ -halfSize, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize, 0.0f,  halfSize, 1, 1, 1, 1 },
	{ -halfSize + 1.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 1.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 2.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 2.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 3.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 3.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 4.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 4.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 5.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 5.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 6.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 6.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 7.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 7.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 8.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 8.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 9.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 9.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 10.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 10.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 11.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 11.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 12.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 12.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 13.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 13.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 14.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 14.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 15.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 15.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 16.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 16.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 17.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 17.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 18.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 18.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 19.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 19.0f, 0.0f, halfSize, 1, 1, 1, 1 },
	{ -halfSize + 20.0f, 0.0f, -halfSize, 1, 1, 1, 1 }, { -halfSize + 20.0f, 0.0f, halfSize, 1, 1, 1, 1 },

	// 가로 선 (X축 방향)
	{ -halfSize, 0.0f, -halfSize, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 1.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 1.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 2.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 2.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 3.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 3.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 4.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 4.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 5.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 5.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 6.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 6.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 7.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 7.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 8.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 8.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 9.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 9.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 10.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 10.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 11.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 11.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 12.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 12.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 13.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 13.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 14.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 14.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 15.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 15.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 16.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 16.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 17.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 17.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 18.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 18.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 19.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 19.0f, 1, 1, 1, 1 },
	{ -halfSize, 0.0f, -halfSize + 20.0f, 1, 1, 1, 1 }, { halfSize, 0.0f, -halfSize + 20.0f, 1, 1, 1, 1 }
};


float cylRadius = 0.08;        // 원기둥 반지름
float cylHeight = 1;        // 원기둥 높이
float coneHeight = 0.5;       // 원뿔 높이
float coneRadius = 0.3;		// 원뿔 반지름

FVertexSimpleOld gizmo_arrow_vertices[] =
{
	// 원기둥 바닥 (총 8개 삼각형)
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },

	// 원기둥 옆면 (총 16개 삼각형)
	{ cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 },

	// 원뿔 밑면 (추가됨, 총 8개 삼각형)
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 },

	// 원뿔 옆면 (총 8개 삼각형)
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 },
	{ 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }
};
