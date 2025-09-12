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
bool ULineBatcherManager::Initialize(ID3D11Device* device, size_t maxLines)
{
    MaxVertices = maxLines * 2; // ★ 누락되어 있던 줄. 한 라인=2정점
    MaxIndices = maxLines * 2;   // 라인당 2 인덱스(양 끝점)


    // === 1. Vertex Buffer ===
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = UINT(sizeof(FLineVertex) * MaxVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&bd, nullptr, &VertexBuffer);

    // 3) IB (R32_UINT로 통일; 6만 정점 이하만 보장되면 R16_UINT로 줄일 수 있음)
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DYNAMIC;
    ibd.ByteWidth = UINT(sizeof(uint32_t) * MaxIndices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&ibd, nullptr, &IndexBuffer);

    // === 4. Constant Buffer (View/Proj) ===
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(ViewProjCB);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    device->CreateBuffer(&cbd, nullptr, &ConstantBuffer);

    // === 3. Compile Shaders ===
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // Vertex Shader
    HRESULT hr = D3DCompileFromFile(L"ShaderLine.hlsl", nullptr, nullptr,"VSmain", "vs_5_0",D3DCOMPILE_ENABLE_STRICTNESS, 0, &vsBlob, nullptr);
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &VertexShader);

    // Pixel Shader
    hr = D3DCompileFromFile(L"ShaderLine.hlsl", nullptr, nullptr, "PSmain", "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &psBlob, nullptr);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &PixelShader);

    // === 4. Input Layout ===
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
          offsetof(FLineVertex, X), D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "COLOR",    0, DXGI_FORMAT_R32_UINT,        0,
          offsetof(FLineVertex, Abgr), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    device->CreateInputLayout(layout, ARRAYSIZE(layout), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &InputLayout);

    if (vsBlob) vsBlob->Release();
    if (psBlob) psBlob->Release();





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

void ULineBatcherManager::AddLine(const FVector& a, const FVector& b, uint32_t color)
{
    // 용량 체크
    if (CpuVertices.size() + 2 > MaxVertices)   return;
    if (CpuIndices.size() + 2 > MaxIndices)  return;

    uint32_t base = (uint32_t)CpuVertices.size();

    CpuVertices.push_back({ a.X, a.Y, a.Z, color });
    CpuVertices.push_back({ b.X, b.Y, b.Z, color });

    CpuIndices.push_back(base + 0);
    CpuIndices.push_back(base + 1);
}

//void ULineBatcherManager::AddBoundingBox(const FBoundingBox& BoundingBox, const FMatrix& WorldMatrix)
//{
//    // 로컬 AABB의 8개 점
//    FVector corners[8] = {
//        { BoundingBox.minX, BoundingBox.minY, BoundingBox.minZ },
//        { BoundingBox.maxX, BoundingBox.minY, BoundingBox.minZ },
//        { BoundingBox.minX, BoundingBox.maxY, BoundingBox.minZ },
//        { BoundingBox.maxX, BoundingBox.maxY, BoundingBox.minZ },
//        { BoundingBox.minX, BoundingBox.minY, BoundingBox.maxZ },
//        { BoundingBox.maxX, BoundingBox.minY, BoundingBox.maxZ },
//        { BoundingBox.minX, BoundingBox.maxY, BoundingBox.maxZ },
//        { BoundingBox.maxX, BoundingBox.maxY, BoundingBox.maxZ }
//    };
//
//    // 월드 변환 적용
//    for (int i = 0; i < 8; i++)
//        corners[i] = WorldMatrix.TransformPosition(corners[i]);
//
//    // 12개의 엣지 연결
//    int edges[12][2] = {
//        {0,1},{0,2},{1,3},{2,3}, // bottom face
//        {4,5},{4,6},{5,7},{6,7}, // top face
//        {0,4},{1,5},{2,6},{3,7}  // vertical edges
//    };
//
//    for (int i = 0; i < 12; i++)
//    {
//        AddLine(corners[edges[i][0]], corners[edges[i][1]], 1);
//    }
//}

void ULineBatcherManager::AddGrid(float spacing, int count, uint32_t colorMain, uint32_t colorAxis)
{
    for (int i = -count; i <= count; i++)
    {
        uint32_t c = (i == 0) ? colorAxis : colorMain;
        AddLine({ i * spacing, -count * spacing, 0 }, { i * spacing, count * spacing, 0 }, c);
        AddLine({ -count * spacing, i * spacing, 0 }, { count * spacing, i * spacing, 0 }, c);
    }
}



/*
CPU에서 쌓인 CpuLines → GPU VertexBuffer에 Map/Unmap으로 복사.

IA에 바인딩 후 Draw로 한 방에 출력.

Topology는 LINELIST라서 (v0,v1), (v2,v3)… 식으로 라인이 나감.
*/
void ULineBatcherManager::Render(ID3D11DeviceContext* ctx, const FMatrix& View, const FMatrix& Proj)
{
    if (CpuIndices.empty()) return;

    D3D11_MAPPED_SUBRESOURCE mapped;
    ctx->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, CpuIndices.data(), sizeof(FLineVertex) * CpuIndices.size());
    ctx->Unmap(VertexBuffer, 0);

    // 2) IB 업로드
    ctx->Map(IndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, CpuIndices.data(), sizeof(uint32_t) * CpuIndices.size());
    ctx->Unmap(IndexBuffer, 0);

    // === 2. View/Proj 상수버퍼 업데이트 ===
    ViewProjCB cb;
    cb.View = View; // HLSL row_major 일 때 맞춰주기
    cb.Proj = Proj;
    ctx->UpdateSubresource(ConstantBuffer, 0, nullptr, &cb, 0, 0);

    // === 3. 파이프라인 바인딩 ===
    UINT stride = sizeof(FLineVertex);
    UINT offset = 0;
    ctx->IASetInputLayout(InputLayout);
    ctx->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    ctx->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    ctx->VSSetShader(VertexShader, nullptr, 0);
    ctx->VSSetConstantBuffers(0, 1, &ConstantBuffer);
    ctx->PSSetShader(PixelShader, nullptr, 0);

    // === 4. Draw ===
    ctx->Draw((UINT)CpuIndices.size(), 0);
}

void ULineBatcherManager::LoadSettings(const char* iniPath)
{
    char buf[64];
    GetPrivateProfileStringA("Grid", "Spacing", "100", buf, 64, iniPath);
    GridSpacing = (float)atof(buf);
}

void ULineBatcherManager::SaveSettings(const char* iniPath)
{
    char buf[64];
    sprintf_s(buf, "%f", GridSpacing);
    WritePrivateProfileStringA("Grid", "Spacing", buf, iniPath);
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