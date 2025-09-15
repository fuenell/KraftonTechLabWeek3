#include "stdafx.h"
#include "UUIDRenderer.h"

bool UUIDRenderer::SetUUIDVertices(
	ID3D11Device* Device,
	float AspectRatio,
	uint32 UUID,
	float RenderSize,
	float ModelScale,
	FMatrix Modeling,
	FMatrix View,
	FMatrix Projection
)
{
	FString UUIDString = FString("UID : ") + std::to_string(UUID);

	FVector4 ObjectCenter = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	ObjectCenter = (Modeling * View).TransformVectorRow(ObjectCenter);
	
	if (ObjectCenter.Z < 0.0f)
		return false;

	ObjectCenter = Projection.TransformVectorRow(ObjectCenter);

	FVector4 RenderCenter = ObjectCenter / ObjectCenter.W;
	
	uint64 StringLen = UUIDString.size();
	float StartPosX = RenderCenter.X - ((float)StringLen * RenderSize * 0.5f * (1 / AspectRatio));

	TArray<FVertexPosColor4> VertexArray;
	TArray<uint32> IndexArray;

	for (uint64 i = 0, s = StringLen; i < s; i++)
	{
		FVertexPosColor4 Vertex1 = {StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f };
		FVertexPosColor4 Vertex2 = {StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f };
		FVertexPosColor4 Vertex3 = {StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, 0.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f };
		FVertexPosColor4 Vertex4 = {StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, 0.0f, 1.0f ,1.0f, 1.0f, 1.0f, 1.0f };

		VertexArray.push_back(Vertex1);
		VertexArray.push_back(Vertex2);
		VertexArray.push_back(Vertex3);
		VertexArray.push_back(Vertex4);

		IndexArray.push_back(0 + 4 * i);
		IndexArray.push_back(1 + 4 * i);
		IndexArray.push_back(2 + 4 * i);
		IndexArray.push_back(2 + 4 * i);
		IndexArray.push_back(1 + 4 * i);
		IndexArray.push_back(3 + 4 * i);
	}

	SpriteManager.SetBuffer(Device, VertexArray, IndexArray);
	return true;
}