// ShaderLine.hlsl
cbuffer ViewProjCB : register(b0)
{
    row_major float4x4 View;
    row_major float4x4 Proj;
}

/*
HLSL의 struct vs_in의 정의 : vs에는 이런 데이터가 들어온다 라는 선언
*/
struct VS_IN
{
    float3 Pos : POSITION;
    uint Color : COLOR0; // DXGI_FORMAT_R32_UINT
};

struct VS_OUT
{
    float4 SvPos : SV_POSITION;
    float4 Color : COLOR0;
};

float4 UnpackRGBA(uint c)
{
    float a = (c & 255) / 255.0;
    float b = ((c >> 8) & 255) / 255.0;
    float g = ((c >> 16) & 255) / 255.0;
    float r = ((c >> 24) & 255) / 255.0;
    return float4(r, g, b, a);
}

VS_OUT VSmain(VS_IN input)
{
    VS_OUT o;
    float4 wp = float4(input.Pos, 1.0);
    float4 vp = mul(wp, View); // row-vector
    o.SvPos = mul(vp, Proj);
    o.Color = UnpackRGBA(input.Color);
    return o;
}

float4 PSmain(VS_OUT input) : SV_TARGET
{
    return input.Color;
}
