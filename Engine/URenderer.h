#pragma once
#include "stdafx.h"
#include "UMesh.h"
#include "Matrix.h"

// URenderer.h or cpp 상단
struct CBTransform
{
    // HLSL의 row_major float4x4와 메모리 호환을 위해 float[16]로 보냄
    float M[16];
    float V[16];
    float P[16];
};
extern CBTransform Transforms;  // 현재 프레임/오브젝트 행렬 캐시

class URenderer
{
private:
    // Core D3D11 objects
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;
    ID3D11DepthStencilView* depthStencilView;
    ID3D11RasterizerState* rasterizerState;

    // Shader objects
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    // Constant buffer
    ID3D11Buffer* constantBuffer;

    // Viewport
    D3D11_VIEWPORT viewport;

    // Window handle
    HWND hWnd;

    // Render state
    bool bIsInitialized;

public:
    URenderer();
    ~URenderer();

    // Initialization and cleanup
    bool Create(HWND windowHandle);
    bool CreateShader();
    bool CreateRasterizerState();
    bool CreateConstantBuffer();
    void Release();
    void ReleaseShader();
    void ReleaseConstantBuffer();

    // Buffer creation
    ID3D11Buffer* CreateVertexBuffer(const void* data, size_t sizeInBytes);
    ID3D11Buffer* CreateIndexBuffer(const void* data, size_t sizeInBytes);
    bool ReleaseVertexBuffer(ID3D11Buffer* buffer);
    bool ReleaseIndexBuffer(ID3D11Buffer* buffer);

    // Texture creation
    ID3D11Texture2D* CreateTexture2D(int width, int height, DXGI_FORMAT format,
        const void* data = nullptr);
    ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Texture2D* texture);
    bool ReleaseTexture(ID3D11Texture2D* texture);
    bool ReleaseShaderResourceView(ID3D11ShaderResourceView* srv);

    // Rendering operations
    void Prepare();
    void PrepareShader();
    void SwapBuffer();
    void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

    // Drawing operations
    void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
    void Draw(UINT vertexCount, UINT startVertexLocation = 0);
    void DrawMesh(UMesh* mesh);

    // Resource binding
    void SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset = 0);
    void SetIndexBuffer(ID3D11Buffer* buffer, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
    void SetConstantBuffer(ID3D11Buffer* buffer, UINT slot = 0);
    void SetTexture(ID3D11ShaderResourceView* srv, UINT slot = 0);

    // Constant buffer updates
    bool UpdateConstantBuffer(const void* data, size_t sizeInBytes);

    // Window resize handling
    bool ResizeBuffers(int width, int height);

    // Getters
    ID3D11Device* GetDevice() const { return device; }
    ID3D11DeviceContext* GetDeviceContext() const { return deviceContext; }
    IDXGISwapChain* GetSwapChain() const { return swapChain; }
    bool IsInitialized() const { return bIsInitialized; }

    // Utility functions
    bool CheckDeviceState();
    void GetBackBufferSize(int& width, int& height);



private:
    // Internal helper functions
    bool CreateDeviceAndSwapChain(HWND windowHandle);
    bool CreateRenderTargetView();
    bool CreateDepthStencilView(int width, int height);
    bool SetupViewport(int width, int height);

    // Error handling
    void LogError(const char* function, HRESULT hr);
    bool CheckResult(HRESULT hr, const char* function);

    // 행렬 복사 핼퍼
    static void CopyRowMajor(const FMatrix& S, float out16[16])
    {
        int k = 0;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                out16[k++] = S.M[r][c];
    }

public:
    void SetViewProj(const FMatrix& V, const FMatrix& P)
    {
        CopyRowMajor(V, Transforms.V);
        CopyRowMajor(P, Transforms.P);
        // 한 번에 보내지 않아도 되지만, 편의상 여기선 안 보냄 (Model 설정 때 같이 보냄)
    }

    void SetModel(const FMatrix& M)
    {
        CopyRowMajor(M, Transforms.M);
        UpdateConstantBuffer(&Transforms, sizeof(Transforms)); // M,V,P 모두 전송
    }
};