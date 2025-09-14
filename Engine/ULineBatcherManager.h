#pragma once
#include "UEngineSubsystem.h"
#include "Vector.h"
#include "Matrix.h"
#include "UMesh.h"

struct FLineVertex {
    float X, Y, Z;       // POSITION
    uint32_t Abgr;     // COLOR packed ABGR (0xAABBGGRR 아님: 본 코드는 ABGR)
};


class ULineBatcherManager : UEngineSubsystem
{
	DECLARE_UCLASS(ULineBatcherManager, UEngineSubsystem)
public:
    bool Initialize(ID3D11Device* InDevice, size_t InMaxLines);
    void BeginFrame();

    // 월드 좌표로 라인 추가 (그리드/스포트라이트/AABB 전부 여기로)
    void AddLine(const FVector& InPointStart, const FVector& InPointEnd, uint32_t InColor);

    // 이미 크기와 위치가 계산된 라인이 들어와서 드로우콜할때 따로 매핑을 신경써주지 않아도 된다. 즉, 여기 밖에서 라인을 계산한다
    void AddBoundingBox(const FBounds& WorldBox, uint32_t color);

    // 그리드도 이제 IB 경로로 쌓음
    void AddGrid(float Spacing, int Count, uint32_t ColorMain, uint32_t ColorAxis); 

    //스포트라이트를 위한 라인을 그리는 함수
    void AddSpotLight(const FVector& InLightPosition, const FMatrix& InMatrix, float InAngle, float InScale);

    void Render(ID3D11DeviceContext* InDeviecContext, const FMatrix& View, const FMatrix& Proj);


    static void LocalAABBtoWorldAABB(const FBounds& L, const FMatrix& InMatrix, FBounds& Out)
    {
        const FVector c = (L.Min + L.Max) * 0.5f;
        const FVector e = (L.Max - L.Min) * 0.5f;

        const FVector cw = InMatrix.TransformPointRow(c);

        // 열 0,1,2를 사용 (row-vector 규약)
        const float m00 = InMatrix.M[0][0], m10 = InMatrix.M[1][0], m20 = InMatrix.M[2][0];
        const float m01 = InMatrix.M[0][1], m11 = InMatrix.M[1][1], m21 = InMatrix.M[2][1];
        const float m02 = InMatrix.M[0][2], m12 = InMatrix.M[1][2], m22 = InMatrix.M[2][2];

        FVector ew;
        ew.X = fabsf(m00) * e.X + fabsf(m10) * e.Y + fabsf(m20) * e.Z;
        ew.Y = fabsf(m01) * e.X + fabsf(m11) * e.Y + fabsf(m21) * e.Z;
        ew.Z = fabsf(m02) * e.X + fabsf(m12) * e.Y + fabsf(m22) * e.Z;

        Out.Min = cw - ew;
        Out.Max = cw + ew;
    }
    static void LocalSphereToWorldAABB(const FVector& InSphereLocation, const FMatrix& InMatrix, FBounds& Out)
    {
        // 1) 중심 이동(평행이동 포함)
        const FVector cw = InSphereLocation;

        // 2) 선형부의 열(column) L2 노름 * r  => 축별 엑스텐트
        const float m00 = InMatrix.M[0][0], m10 = InMatrix.M[1][0], m20 = InMatrix.M[2][0]; // col 0
        const float m01 = InMatrix.M[0][1], m11 = InMatrix.M[1][1], m21 = InMatrix.M[2][1]; // col 1
        const float m02 = InMatrix.M[0][2], m12 = InMatrix.M[1][2], m22 = InMatrix.M[2][2]; // col 2

        FVector ew;
        ew.X = sqrtf(m00 * m00 + m10 * m10 + m20 * m20);
        ew.Y = sqrtf(m01 * m01 + m11 * m11 + m21 * m21);
        ew.Z = sqrtf(m02 * m02 + m12 * m12 + m22 * m22);

        // 3) 최종 AABB
        Out.Min = cw - ew;
        Out.Max = cw + ew;
    }

    


    // getter
    inline float GetGridSpace() const { return GridSpacing; }

    // setter
    inline void SetGridSpace(float InGridSpace) { GridSpacing = InGridSpace; }

    // Grid spacing 저장/로드
    void LoadSettings(const char* IniPath);
    void SaveSettings(const char* IniPath);

    void Release();

    float GridSpacing = 1.0f;

private:
    TArray<FLineVertex> CpuVertices;
    TArray<uint32_t>    CpuIndices;

    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    ID3D11Buffer* ConstantBuffer = nullptr;

    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;

    size_t MaxVertices = 0, MaxIndices = 0;
};

