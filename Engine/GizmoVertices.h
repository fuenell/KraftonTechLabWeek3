#pragma once
#include "FVertexPosColor.h"
#include "TArray.h"

extern const TArray<FVertexPosColor> gizmo_arrow_vertices;

extern const TArray<FVertexPosColor> gizmo_rotation_handle_vertices;

extern const TArray<FVertexPosColor> gizmo_scale_handle_vertices;

class GridGenerator
{
public:
	static TArray<FVertexPosColor> CreateGridVertices(float gridSize, int gridCount);
	static TArray<FVertexPosColor> CreateRotationHandleVertices();
};
