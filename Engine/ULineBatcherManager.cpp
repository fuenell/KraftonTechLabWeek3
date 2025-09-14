#include "stdafx.h"
#include "ULineBatcherManager.h"

IMPLEMENT_UCLASS(ULineBatcherManager, UEngineSubsystem)
struct ViewProjCB
{
    FMatrix View;
    FMatrix Proj;
};

/*
정점 버퍼 생성
D3D11_USAGE_DYNAMIC 버퍼를 만들어서 매 프레임 CPU에서 갱신할 수 있게 함.
최대 라인 개수(maxLines)를 받아서 버퍼 크기 계산.
*/
bool ULineBatcherManager::Initialize(ID3D11Device* InDevice, size_t MaxLines)
{
    MaxVertices = MaxLines * 2; // ★ 누락되어 있던 줄. 한 라인=2정점
    MaxIndices = MaxLines * 2;   // 라인당 2 인덱스(양 끝점)


    // === 1. Vertex Buffer ===
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = UINT(sizeof(FLineVertex) * MaxVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    InDevice->CreateBuffer(&bd, nullptr, &VertexBuffer);

    // 3) IB (R32_UINT로 통일; 6만 정점 이하만 보장되면 R16_UINT로 줄일 수 있음)
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.ByteWidth = UINT(sizeof(uint32_t) * MaxIndices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    InDevice->CreateBuffer(&ibd, nullptr, &IndexBuffer);

    // === 3) Constant Buffer ===
    {
        D3D11_BUFFER_DESC cbd{};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.ByteWidth = sizeof(ViewProjCB);
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        InDevice->CreateBuffer(&cbd, nullptr, &ConstantBuffer);
    }

    // === 4) 셰이더/레이아웃 ===
    {
        ID3DBlob* vsBlob = nullptr;
        ID3DBlob* psBlob = nullptr;

        D3DCompileFromFile(L"ShaderLine.hlsl", nullptr, nullptr, "VSmain", "vs_5_0",
            D3DCOMPILE_ENABLE_STRICTNESS, 0, &vsBlob, nullptr);
        InDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            nullptr, &VertexShader);

        D3DCompileFromFile(L"ShaderLine.hlsl", nullptr, nullptr, "PSmain", "ps_5_0",
            D3DCOMPILE_ENABLE_STRICTNESS, 0, &psBlob, nullptr);
        InDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
            nullptr, &PixelShader);

        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
              offsetof(FLineVertex, X), D3D11_INPUT_PER_VERTEX_DATA, 0 },

              // ABGR를 uint로 줄 거면 R32_UINT로, HLSL에서 uint COLOR 받아서 float4로 변환
              { "COLOR",    0, DXGI_FORMAT_R32_UINT,        0,
                offsetof(FLineVertex, Abgr), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        InDevice->CreateInputLayout(layout, ARRAYSIZE(layout),
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            &InputLayout);

        if (vsBlob) vsBlob->Release();
        if (psBlob) psBlob->Release();
    }
    return true;
}


/*
CpuLines 벡터 클리어.
이후 프레임 동안 들어오는 AddLine 요청들이 여기에 쌓임.
*/
void ULineBatcherManager::BeginFrame()
{
    CpuVertices.clear();
    CpuIndices.clear();
}

void ULineBatcherManager::AddLine(const FVector& InPointStart, const FVector& InPointEnd, uint32_t InColor)
{
    // 용량 체크
    if (CpuVertices.size() + 2 > MaxVertices)   return;
    if (CpuIndices.size() + 2 > MaxIndices)  return;

    // 추가하기 전에 쌓여있는 정점의 사이즈
    uint32_t base = (uint32_t)CpuVertices.size();

    CpuVertices.push_back({ InPointStart.X, InPointStart.Y, InPointStart.Z, InColor });
    CpuVertices.push_back({ InPointEnd.X, InPointEnd.Y, InPointEnd.Z, InColor });

    CpuIndices.push_back(base + 0);
    CpuIndices.push_back(base + 1);
}

void ULineBatcherManager::AddBoundingBox(const FBounds& WorldBox, uint32_t color)
{
    // 월드 AABB의 8개 코너 (Min/Max가 모두 월드 좌표)
    FVector corners[8] = {
        { WorldBox.Min.X, WorldBox.Min.Y, WorldBox.Min.Z },
        { WorldBox.Max.X, WorldBox.Min.Y, WorldBox.Min.Z },
        { WorldBox.Min.X, WorldBox.Max.Y, WorldBox.Min.Z },
        { WorldBox.Max.X, WorldBox.Max.Y, WorldBox.Min.Z },
        { WorldBox.Min.X, WorldBox.Min.Y, WorldBox.Max.Z },
        { WorldBox.Max.X, WorldBox.Min.Y, WorldBox.Max.Z },
        { WorldBox.Min.X, WorldBox.Max.Y, WorldBox.Max.Z },
        { WorldBox.Max.X, WorldBox.Max.Y, WorldBox.Max.Z }
    };

    // 12개의 엣지 인덱스 (LINELIST로 그릴 선분)
    static const int edges[12][2] = {
        {0,1},{0,2},{1,3},{2,3}, // “아랫면” (MinZ)
        {4,5},{4,6},{5,7},{6,7}, // “윗면”   (MaxZ)
        {0,4},{1,5},{2,6},{3,7}  // 수직 엣지
    };

    for (int i = 0; i < 12; ++i)
    {
        AddLine(corners[edges[i][0]], corners[edges[i][1]], color);
    }
}

void ULineBatcherManager::AddGrid(float InSpacing, int InCount, uint32_t InColorMain, uint32_t InColorAxis)
{
    // ABGR(0xAABBGGRR) 기준
    const uint32_t Red = 0xFF0000FF; // X축
    const uint32_t Green = 0x00FF00FF; // Y축
    const uint32_t Blue = 0x0000FFFF; // Z축

    // 그리드

    const float Ext = InCount * InSpacing;
    for (int i = -InCount; i <= InCount; i++)
    {
        const float X = i * InSpacing;
        const float Y = i * InSpacing;

        //  Y축
        uint32_t ColVert = (i == 0) ? Green : InColorMain;
        AddLine({ X, -Ext, 0.0f }, { X,  Ext, 0.0f }, ColVert);

        //  X축 
        uint32_t ColHorz = (i == 0) ? Red : InColorMain;
        AddLine({ -Ext, Y, 0.0f }, { Ext, Y, 0.0f }, ColHorz);
    }
    // Z축(파랑) — 원점에서 ±Z로 한 줄
    AddLine({ 0.0f, 0.0f, -Ext }, { 0.0f, 0.0f,  Ext }, Blue);

    // spotlight
     
    // bounding box

}


void ULineBatcherManager::AddSpotLight(const FVector& InLightPosition, const FMatrix& InMatrix, float InAngle, float InScale)
{
    // 여기서 라인 생성해서 FArray에 넣어줌
    // 기저 벡터 구하기
    //FVector Forward = InForward;
    //Forward.Normalize();
    //const FVector upGuess = (fabsf(Forward.Z) < 0.999f) ? FVector(0, 1, 0) : FVector(0, 0, 1);
    //FVector Right = upGuess.Cross(Forward);  Right.Normalize();
    //FVector Up = Forward.Cross(Right);    // 자동 직교
    //
    //
    //// 3) 로컬→월드 변환 행렬(M) 구성 (row-vector 규약)
    //// p_world = [px py pz 1] * M
    //FMatrix M = FMatrix::IdentityMatrix();
    //// 로컬 x/y/z 기저를 월드로: 각 "행"에 기저벡터를 둔다
    //M.M[0][0] = Forward.X;  M.M[0][1] = Forward.Y;  M.M[0][2] = Forward.Z;
    //M.M[1][0] = Right.X;     M.M[1][1] = Right.Y;     M.M[1][2] = Right.Z;
    //M.M[2][0] = Up.X;      M.M[2][1] = Up.Y;      M.M[2][2] = Up.Z;
    //// 평행이동 (row-vector에서는 마지막 행)
    //M.M[3][0] = InLightPosition.X;    M.M[3][1] = InLightPosition.Y;    M.M[3][2] = InLightPosition.Z;



    // 추가하기 전에 쌓여있는 정점의 사이즈
    uint32_t base = (uint32_t)CpuVertices.size();

    // 용량 체크
    if (CpuVertices.size() + 2 > MaxVertices)   return;
    if (CpuIndices.size() + 2 > MaxIndices)  return;



    const uint32_t InColor = 0xFFFFFF80;


    //우선 시작점 (location)을 버텍스에 넣음 
    CpuVertices.push_back({ InLightPosition.X, InLightPosition.Y, InLightPosition.Z, InColor });
    //CpuIndices.push_back(base);


    // 여기서 계산식 토대로 라인들을 넣음
    for (int32_t i = 0; i < 360; ++i)
    {
        const float theta = i * DegreeToRadian;
        const float L = InScale;                              // 원뿔 길이(앞으로)
        const float r = L * tanf(InAngle * DegreeToRadian); // 끝 원 반지름
        FVector4 PLocal(L, r * cosf(theta), r * sinf(theta), 1.0f);

        //FVector4 NewSpot = { Forward.X * InScale, Forward.X * InScale * sinf(i * DegreeToRadian), Forward.X * InScale * tanf(i * DegreeToRadian) * sinf(i * DegreeToRadian), 1.0f };

        //여기에 변환식들어가야함
        PLocal = FMatrix::MultiplyVectorRow(PLocal, InMatrix);

        CpuVertices.push_back({ PLocal.X, PLocal.Y, PLocal.Z, InColor });

        CpuIndices.push_back(base);
        CpuIndices.push_back(base + i + 1);
    }


    // spot light의 끝부분 원을 표현하기 위함
    for (int32_t i = 0; i < 359; ++i)
    {
        CpuIndices.push_back(base + i + 1);
        CpuIndices.push_back(base + i + 2);
    }
    CpuIndices.push_back(base + 359);
    CpuIndices.push_back(base + 1);

}

/*
CPU에서 쌓인 CpuLines → GPU VertexBuffer에 Map/Unmap으로 복사.

IA에 바인딩 후 Draw로 한 방에 출력.

Topology는 LINELIST라서 (v0,v1), (v2,v3)… 식으로 라인이 나감.
*/
void ULineBatcherManager::Render(ID3D11DeviceContext* InDeviceContext, const FMatrix& View, const FMatrix& Proj)
{
    if (CpuIndices.empty()) return;

    D3D11_MAPPED_SUBRESOURCE Mapped;
    InDeviceContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
    memcpy(Mapped.pData, CpuVertices.data(), sizeof(FLineVertex) * CpuVertices.size());
    InDeviceContext->Unmap(VertexBuffer, 0);

    // 2) IB 업로드
    InDeviceContext->Map(IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Mapped);
    memcpy(Mapped.pData, CpuIndices.data(), sizeof(uint32_t) * CpuIndices.size());
    InDeviceContext->Unmap(IndexBuffer, 0);

    // === 2. View/Proj 상수버퍼 업데이트 ===
    ViewProjCB Cb;
    Cb.View = View; // HLSL row_major 일 때 맞춰주기
    Cb.Proj = Proj;
    InDeviceContext->UpdateSubresource(ConstantBuffer, 0, nullptr, &Cb, 0, 0);

    // === 3. 파이프라인 바인딩 ===
    UINT Stride = sizeof(FLineVertex);
    UINT Offset = 0;
    InDeviceContext->IASetInputLayout(InputLayout);
    InDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &Stride, &Offset);
    InDeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    InDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    InDeviceContext->VSSetShader(VertexShader, nullptr, 0);
    InDeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    InDeviceContext->PSSetShader(PixelShader, nullptr, 0);

    // === 4. Draw ===
    InDeviceContext->DrawIndexed((UINT)CpuIndices.size(), 0, 0);
}

void ULineBatcherManager::LoadSettings(const char* IniPath)
{
    char buf[64];
    GetPrivateProfileStringA("Grid", "Spacing", "100", buf, 64, IniPath);
    GridSpacing = (float)atof(buf);
}

void ULineBatcherManager::SaveSettings(const char* IniPath)
{
    char buf[64];
    sprintf_s(buf, "%f", GridSpacing);
    WritePrivateProfileStringA("Grid", "Spacing", buf, IniPath);
}

void ULineBatcherManager::Release()
{
    CpuVertices.clear();
    CpuIndices.clear();

    if (InputLayout) { InputLayout->Release();    InputLayout = nullptr; }
    if (PixelShader) { PixelShader->Release();    PixelShader = nullptr; }
    if (VertexShader) { VertexShader->Release();   VertexShader = nullptr; }
    if (ConstantBuffer) { ConstantBuffer->Release(); ConstantBuffer = nullptr; }
    if (IndexBuffer) { IndexBuffer->Release();    IndexBuffer = nullptr; }
    if (VertexBuffer) { VertexBuffer->Release();   VertexBuffer = nullptr; }

}




//gLine.BeginFrame();
//
//// 1) 그리드 쌓기 (IB 경로로)
//gLine.AddGrid(gLine.GridSpacing, /*count*/ 200, colMain, colAxis);
//
//// 2) 스포트라이트/AABB도 월드 좌표로 정점 만들고 gLine.AddLine(...) 반복
//
//// 3) 마지막에 1콜
//gLine.Flush(ctx, View, Proj);