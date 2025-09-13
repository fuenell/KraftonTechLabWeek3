#pragma once
#include "UEngineSubsystem.h"
#include "Vector.h"
#include "Matrix.h"

struct FLineVertex {
    float X, Y, Z;       // POSITION
    uint32_t Abgr;     // COLOR packed ABGR (0xAABBGGRR 아님: 본 코드는 ABGR)
};

// AABB 구조체
struct FBoundingBox
{
    float MinX, MinY, MinZ;
    float MaxX, MaxY, MaxZ;
};
class ULineBatcherManager : UEngineSubsystem
{
	DECLARE_UCLASS(ULineBatcherManager, UEngineSubsystem)
public:
    bool Initialize(ID3D11Device* InDevice, size_t InMaxLines);
    void BeginFrame();

    // 월드 좌표로 라인 추가 (그리드/스포트라이트/AABB 전부 여기로)
    void AddLine(const FVector& a, const FVector& b, uint32_t color);
    //void AddBoundingBox(const FBoundingBox& BoundingBox, const FMatrix& WorldMatrix);

    // 그리드도 이제 IB 경로로 쌓음
    void AddGrid(float spacing, int count, uint32_t colorMain, uint32_t colorAxis);

    static void BuildGridGeometry(int halfCount, float spacing, uint32_t colorMain, uint32_t colorAxis, TArray<FLineVertex>& outVerts, std::vector<uint32_t>& outInds);
   

    void Render(ID3D11DeviceContext* ctx, const FMatrix& View, const FMatrix& Proj);

    // Grid spacing 저장/로드
    void LoadSettings(const char* iniPath);
    void SaveSettings(const char* iniPath);

    float GridSpacing = 1.0f;

private:
    TArray<FLineVertex> CpuVertices;
    TArray<uint32_t>    CpuIndices;

    // Indexed (스포트라이트, AABB)
    TArray<FLineVertex> CpuVertsIB;

    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    ID3D11Buffer* ConstantBuffer = nullptr;

    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;

    size_t MaxVertices = 0, MaxIndices = 0;
};

