#include "Line3d.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gWorldViewProjection : register(b0);

struct VS_Input
{
    float t : LOCAL;
    float3 startPos : STARTPOS;
    float3 endPos : ENDPOS;
};

VS_Output main(VS_Input input)
{
    float3 world = lerp(input.startPos, input.endPos, input.t);
    VS_Output output;

    output.position = mul(float4(world, 1.0f), gWorldViewProjection.WVP);

    return output;
}