struct VS_INPUT {
    float4 Position : POSITION;
    float4 Color    : COLOR;
};
struct VS_OUTPUT {
    float4 Position : SV_POSITION;
    float4 Color    : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = input.Position;
    output.Color = input.Color;
    
    return output;
}