//--------------------
// 버텍스 입력 구조체
//--------------------
struct VSInput
{
    float3 position : POSITION; // 정점 좌표
    float2 texCoord : TEXCOORD; // 글리프 UV
};

//--------------------
// 버텍스 셰이더 출력 구조체
//--------------------
struct VSOutput
{
    float4 position : SV_Position; // 변환된 좌표
    float2 texCoord : TEXCOORD; // 텍스처 좌표
};

//--------------------
// 버텍스 셰이더
// (위치 그대로 넘기고, UV만 전달)
//--------------------
VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 1.0f);
    output.texCoord = input.texCoord;
    return output;
}

//--------------------
// 픽셀 셰이더
//--------------------
Texture2D fontTex : register(t0); // 폰트 아틀라스 텍스처
SamplerState samp : register(s0); // 샘플러 상태

float4 PSMain(VSOutput input) : SV_Target
{
    float4 texColor = fontTex.Sample(samp, input.texCoord); // VS에서 넘겨준 UV 값을 통해 텍스처에서 텍셀 정보를 가져옴
    
    if (texColor.r < 0.2f && texColor.g < 0.2f && texColor.b < 0.2f)
        discard;
    
    //return float4(input.texCoord, 0, 1);
    // 알파 채널이 글자 모양 (mask)
    //float4 targetColor = float4(1, 0, 0, 1); // 빨간색
    
    return texColor;
    
    //return float4(1, 1, 1, 1);

}