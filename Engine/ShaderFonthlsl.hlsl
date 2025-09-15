struct VSInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD;
};


VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}


Texture2D fontTex : register(t0);
SamplerState samp : register(s0);

float4 PSMain(VSOutput input) : SV_Target
{
    float4 texColor = fontTex.Sample(samp, input.texCoord);
    
    if (texColor.r < 0.1f && texColor.g < 0.1f && texColor.b < 0.1f)
        discard;
    //return float4(input.texCoord, 0, 1);
    // 알파 채널이 글자 모양 (mask)
    return texColor;

}