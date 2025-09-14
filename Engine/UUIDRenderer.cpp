#include "stdafx.h"
#include "UUIDRenderer.h"

bool UUIDRenderer::Initialize(ID3D11Device* Device)
{
	return BBManager.Initialize(Device);
}

void UUIDRenderer::SetUUIDVertices(
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
	FMatrix MVP = Modeling * View * Projection;
	FMatrix MV = Modeling * View;

	FString UUIDString = std::to_string(UUID);
	OutputDebugStringA((UUIDString + "\n").c_str());

	FVector4 ObjectCenter = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	/*FVector4 ObjectBoundary = MV.TransformVectorRow(ObjectCenter);
	ObjectBoundary.Z += 0.5f * ModelScale;
	ObjectBoundary = Projection.TransformVectorRow(ObjectBoundary);*/
	
	ObjectCenter = MVP.TransformVectorRow(ObjectCenter);

	/*float CenterToBoundary =
		(
			(ObjectBoundary / ObjectBoundary.W) -
			(ObjectCenter / ObjectCenter.W)
		).Length();*/

	FVector4 RenderCenter = ObjectCenter / ObjectCenter.W;
	/*RenderCenter.Y += CenterToBoundary;*/
	
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

	BBManager.SetBuffer(Device, VertexArray, IndexArray);
}

void UUIDRenderer::Bind(ID3D11DeviceContext* DeviceContext)
{
	BBManager.Bind(DeviceContext);
}

void UUIDRenderer::Render(ID3D11DeviceContext* DeviceContext)
{
	BBManager.Render(DeviceContext);
}

void UUIDRenderer::Release()
{
	BBManager.Release();
}