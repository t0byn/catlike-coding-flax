#include "./Flax/Common.hlsl"

META_CB_BEGIN(0, Data)
float4x4 MVP;
float4 Tint;
float4 MainTexST;
float4 DetailTexST;
META_CB_END

SamplerState MainTexSampler : register(s6);
Texture2D MainTex : register(t0);
Texture2D DetailTex : register(t1);

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float2 uvDetail : TEXCOORD1;
};

META_VS(true, FEATURE_LEVEL_ES2)
VS_OUTPUT MyVertexProgram(float3 position : POSITION, float2 uv : TEXCOORD0)
{
    VS_OUTPUT output;
    output.position = mul(MVP, float4(position, 1));
    output.uv = uv * MainTexST.xy + MainTexST.zw;
    output.uvDetail = uv * DetailTexST.xy + DetailTexST.zw;
    return output;
}

META_PS(true, FEATURE_LEVEL_ES2)
float4 MyFragmentProgram(VS_OUTPUT input) : SV_Target
{
    float4 color = MainTex.Sample(MainTexSampler, input.uv) * Tint;
    color *= DetailTex.Sample(MainTexSampler, input.uvDetail) * 2;
    return color;
}
