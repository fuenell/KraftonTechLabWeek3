#pragma once
#include "FVertexPosColor.h"
#include "Globals.h"

extern const TArray<FVertexPosColor> gizmo_arrow_vertices;

class GridGenerator
{
public:
	static TArray<FVertexPosColor> CreateGridVertices(float gridSize, int gridCount);
};
