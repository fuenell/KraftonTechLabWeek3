// VS
cbuffer ConstantBuffer : register(b0) // b0은 0번째 상수 버퍼라는 뜻
{
    row_major float4x4 MVP; // Model   (row-vector)
    float4 MeshColor;
    float IsSelected;
    float padding[3];
};

struct VS_INPUT
{
    float4 Position : POSITION;
    float4 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

VS_OUTPUT mainVS(VS_INPUT input)
{
    VS_OUTPUT output;

    float4 baseColor = input.Color;

    if (0.5f < IsSelected)
    {
        baseColor.rgb = baseColor.rgb + 0.25f;
    }
    

    output.Color = baseColor * MeshColor;

    float4 wpos = float4(input.Position.xyz, 1.0f);

    // row: v' = v * MVP
    output.Position = mul(wpos, MVP);

    return output;
}

// PS
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 mainPS(PS_INPUT input) : SV_Target
{
    return input.Color;
}