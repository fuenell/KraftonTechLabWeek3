#include "stdafx.h"
#include "GizmoVertices.h"

const float cylRadius = 0.025;        // 원기둥 반지름
const float cylHeight = 1;        // 원기둥 높이
const float coneHeight = 0.25;       // 원뿔 높이
const float coneRadius = 0.125;		// 원뿔 반지름

const TArray<FVertexPosColor> gizmo_arrow_vertices =
{
	// 원기둥 바닥 (총 8개 삼각형, 정점 순서 뒤집힘)
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, 0.0f, 1, 1, 1, 1 },

	// 원기둥 옆면 (총 16개 삼각형)
	{ cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 }, { 0.0f, 0.0f, cylRadius, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },
	{ -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 }, { -cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 }, { 0.0f, 0.0f, -cylRadius, 1, 1, 1, 1 },
	{ cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -cylRadius, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, 0.0f, -cylRadius * 0.7071f, 1, 1, 1, 1 },
	{ cylRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { cylRadius * 0.7071f, cylHeight, -cylRadius * 0.7071f, 1, 1, 1, 1 }, { cylRadius, 0.0f, 0.0f, 1, 1, 1, 1 },

	// 원뿔 밑면 (추가됨, 총 8개 삼각형, 정점 순서 뒤집힘)
	{ coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },
	{ coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, 0.0f, 1, 1, 1, 1 },

	// 원뿔 옆면 (총 8개 삼각형, 정점 순서 다시 뒤집힘)
	{ coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, coneRadius, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, coneRadius * 0.7071f, 1, 1, 1, 1 }, { -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ -coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ 0.0f, cylHeight, -coneRadius, 1, 1, 1, 1 }, { coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 },
	{ coneRadius * 0.7071f, cylHeight, -coneRadius * 0.7071f, 1, 1, 1, 1 }, { coneRadius, cylHeight, 0.0f, 1, 1, 1, 1 }, { 0.0f, cylHeight + coneHeight, 0.0f, 1, 1, 1, 1 }
};

// 기즈모 크기 상수 정의
const float boxWidth = 0.05f; // 사각기둥의 너비 (X, Z 방향)
const float boxHeight = 1.0f; // 사각기둥의 높이 (Y 방향)
const float cubeSize = 0.2f;  // 정육면체의 한 변 길이
const float cubeOffset = boxHeight + (cubeSize / 2.0f); // 정육면체 중심의 Y 오프셋 (사각기둥 끝에 붙이도록)

const TArray<FVertexPosColor> gizmo_scale_handle_vertices =
{
	// ===================================================================================
	// 사각기둥 (Box/Prism) - 12개 삼각형 (정점 순서 반전)
	// ===================================================================================

	// 아랫면 (-Y)
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 },
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 },

	// 윗면 (+Y, 사각기둥의 끝)
	{ -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 },
	{ -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 },

	// 옆면 1 (앞, +Z)
	{ -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 },
	{ -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 },

	// 옆면 2 (뒤, -Z)
	{ boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 },
	{ boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 },

	// 옆면 3 (오른쪽, +X)
	{ boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 },
	{ boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 }, { boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 },

	// 옆면 4 (왼쪽, -X)
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, 0.0f, boxWidth / 2.0f, 1, 1, 1, 1 },
	{ -boxWidth / 2.0f, 0.0f, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, -boxWidth / 2.0f, 1, 1, 1, 1 }, { -boxWidth / 2.0f, boxHeight, boxWidth / 2.0f, 1, 1, 1, 1 },

	// ===================================================================================
	// 정육면체 (Cube) - 12개 삼각형 (정점 순서 반전)
	// ===================================================================================

	// 아랫면 (정육면체 기준 -Y)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },

	// 윗면 (정육면체 기준 +Y)
	{ -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 1 (앞, +Z)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 2 (뒤, -Z)
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 3 (오른쪽, +X)
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },
	{ cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 },

	// 옆면 4 (왼쪽, -X)
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 },
	{ -cubeSize / 2.0f, cubeOffset - cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, -cubeSize / 2.0f, 1, 1, 1, 1 }, { -cubeSize / 2.0f, cubeOffset + cubeSize / 2.0f, cubeSize / 2.0f, 1, 1, 1, 1 }
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
