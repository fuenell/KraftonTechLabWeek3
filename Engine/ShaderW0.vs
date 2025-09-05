// ShaderW0.vs
struct VS_INPUT {
    float4 Position : POSITION;
    float4 Color    : COLOR;
};
struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float3 Color    : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    row_major float4x4 MVP; // Model   (row-vector 규약)
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT o;
    float4 wpos = float4(input.Position.xyz, 1.0f);
    // row 규약: v' = v * MVP
    o.Position = mul( wpos, MVP );
    o.Color = input.Color.rgb;
    return o;
}