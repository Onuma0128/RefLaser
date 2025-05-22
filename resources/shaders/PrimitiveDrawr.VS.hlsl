#include "PrimitiveDrawr.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    float2 texcoord = input.texcoord;
    texcoord.y = 1.0f - texcoord.y;
    output.texcoord = texcoord;
    return output;
}
