#include "./Flax/Common.hlsl"

#define TRANSFORM_TEX(tex,name) (tex.xy * name##_ST.xy + name##_ST.zw)

META_CB_BEGIN(0, Data)
float4x4 MatrixMVP;
float4 Tint;
float4 MainTex_ST;
META_CB_END

Texture2D MainTex : register(t0);

SamplerState MainTexSamplerState : register(s0);

struct Interpolators
{
    float4 position : SV_Position;
    //float3 localPosition : TEXCOORD0;
    float2 uv : TEXCOORD0;
};

META_VS(true, FEATURE_LEVEL_ES2)
Interpolators MyVertexProgram(float3 position : POSITION, float2 uv : TEXCOORD0)
{
    Interpolators i;
    i.position = mul(MatrixMVP, float4(position, 1));
    //Flax Engine use cm as its base unit, while Unity use m. To match Unity's result, divide by 100;
    //i.localPosition = position / 100;
    //i.uv = uv;
    //i.uv = TRANSFORM_TEX(uv, MainTex);
    i.uv = uv * MainTex_ST.xy + MainTex_ST.zw;
    return i;
}

META_PS(true, FEATURE_LEVEL_ES2)
float4 MyFragmentProgram(Interpolators i) : SV_Target
{
    //return Tint;

    //return float4(i.localPosition + 0.5, 1) * Tint;

    //return float4(i.uv, 1, 1);

    return MainTex.Sample(MainTexSamplerState, i.uv) * Tint;
}

/*
#include "./Flax/Common.hlsl"

//#pragma pack_matrix(row_major)

META_CB_BEGIN(0, Data)
float4x4 MatrixMVP;
float4 Tint;
META_CB_END

META_VS(true, FEATURE_LEVEL_ES2)
float4 MyVertexProgram(float3 position : POSITION, out nointerpolation float3 localPosition : TEXCOORD0) : SV_Position
{
    //position /= 100;
    //position.z += 0.5; // DirectX's NDK z axis range is [0, 1]
    //return float4(position, 1);

    //return mul(float4(position, 1), MatrixMVP);
    //use `mul(Matrix, Vector)` instead because the matrix is stored in column-major

    //Flax Engine use cm as its base unit, while Unity use m. To match Unity's result, divide by 100;
    localPosition = position / 100;
    return mul(MatrixMVP, float4(position, 1));
}

META_PS(true, FEATURE_LEVEL_ES2)
float4 MyFragmentProgram(float4 position : SV_Position, nointerpolation float3 localPosition : TEXCOORD0) : SV_Target
{
    //return Tint;

    return float4(localPosition, 1);
}
*/
