#pragma once
#include "FVertexPosColor.h"
#include "TArray.h"

extern const TArray<FVertexPosColor> GizmoArrowVertices;

extern const TArray<FVertexPosColor> GizmoScaleHandleVertices;

class GridGenerator
{
public:
	//static TArray<FVertexPosColor> CreateGridVertices(float gridSize, int32 gridCount);
	static TArray<FVertexPosColor> CreateRotationHandleVertices();
};
