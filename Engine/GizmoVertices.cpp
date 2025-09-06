#include "stdafx.h"
#include "GizmoVertices.h"

const float cylRadius = 0.08;        // 원기둥 반지름
const float cylHeight = 1;        // 원기둥 높이
const float coneHeight = 0.5;       // 원뿔 높이
const float coneRadius = 0.3;		// 원뿔 반지름

const TArray<FVertexPosColor> gizmo_arrow_vertices =
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

TArray<FVertexPosColor> GridGenerator::CreateGridVertices(float gridSize, int gridCount)
{
	// TArray 컨테이너 생성
	TArray<FVertexPosColor> vertices;

	// Z축에 평행한 세로 선들을 생성하는 로직
	for (int i = -gridCount; i <= gridCount; ++i)
	{
		vertices.push_back({ -gridSize * gridCount, 0.0f, i * gridSize, 1.0f, 1.0f, 1.0f, 1.0f });
		vertices.push_back({ gridSize * gridCount, 0.0f, i * gridSize, 1.0f, 1.0f, 1.0f, 1.0f });
	}

	// X축에 평행한 가로 선들을 생성하는 로직
	for (int i = -gridCount; i <= gridCount; ++i)
	{
		vertices.push_back({ i * gridSize, 0.0f, -gridSize * gridCount, 1.0f, 1.0f, 1.0f, 1.0f });
		vertices.push_back({ i * gridSize, 0.0f, gridSize * gridCount, 1.0f, 1.0f, 1.0f, 1.0f });
	}

	FVertexPosColor::ChangeAxis(vertices.data(), (int)vertices.size(), 1, 2);

	return vertices;
}
