#include "stdafx.h"
#include "TestUUIDRenderer.h"

bool TestUUIDRenderer::SetUUIDVertices(
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
	FString UUIDString = FString("ABC : ") + std::to_string(UUID);

	FVector4 ObjectCenter = { 0.0f, 0.0f, 0.0f, 1.0f };

	ObjectCenter = (Modeling * View).TransformVectorRow(ObjectCenter);

	if (ObjectCenter.Z < 0.0f)
		return false;

	ObjectCenter = Projection.TransformVectorRow(ObjectCenter);

	FVector4 RenderCenter = ObjectCenter / ObjectCenter.W;

	uint64 StringLen = UUIDString.size();
	float StartPosX = RenderCenter.X - ((float)StringLen * RenderSize * 0.5f * (1 / AspectRatio));

	TArray<FVertexPosUV> VertexArray;
	TArray<uint32> IndexArray;

	for (uint64 i = 0, s = StringLen; i < s; i++)
	{
		CharacterInfo charInfo = Atlas.GetCharInfo(UUIDString[s]);

		float u = charInfo.u;
		float v = charInfo.v;
		float width = charInfo.height;
		float height = charInfo.width;

		FVertexPosUV Vertex1 = { StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, u, v + height };
		FVertexPosUV Vertex2 = { StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y, 0.0f, u + width, v + height};
		FVertexPosUV Vertex3 = { StartPosX + i * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, u, v };
		FVertexPosUV Vertex4 = { StartPosX + (i + 1) * RenderSize * (1 / AspectRatio), RenderCenter.Y + (float)RenderSize, 0.0f, u + width, v };

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

	SpriteManager.SetBufferUV(Device);

	return true;
}