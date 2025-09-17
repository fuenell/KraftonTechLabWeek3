
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 texCoord : TEXCOORD;
};
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float2 texCoord;
};

cbuffer ViewProjCB : register(b0)
{
    row_major float4x4 MVP;
    // 텍스처의 한 변에 이미지가 몇 장이나 있는가
    uint CellNumInRow;
    // 몇 번째 이미지를 그려야 하는가
    uint CurrentCellIndex;
}

VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Position = mul(float4(input.Position, 1.0f), MVP);
    
    float cellSize = 1.0f / (float) CellNumInRow;
    uint col = CurrentCellIndex % CellNumInRow;
    uint row = CurrentCellIndex / CellNumInRow;
    
    float2 cellOffset = float2(col * cellSize, row * cellSize);
    
    // 입력 UV를 현재 셀에 맞게 변환
    output.texCoord = input.texCoord * cellSize + cellOffset;
    
    return output;
}

Texture2D fontTex : register(t0); // 폰트 아틀라스 텍스처
SamplerState samp : register(s0); // 샘플러 상태

float4 PS_Main(VS_OUTPUT input) : SV_Target
{
    float4 texColor = fontTex.Sample(samp, input.texCoord); // VS에서 넘겨준 UV 값을 통해 텍스처에서 텍셀 정보를 가져옴
    
    if (texColor.r < 0.3f && texColor.g < 0.3f && texColor.b < 0.3f)
        discard;
    
     return texColor;
    //return (1, 1, 1, 1);
    
}