#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
sampler gSampler : register(s0);

struct PixelShaderOutpot
{
    float4 color : SV_TARGET0;
};

PixelShaderOutpot main(VertexShaderOutput input)
{
    PixelShaderOutpot output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    return output;
}