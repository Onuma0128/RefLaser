#include "Fullscreen.hlsli"

struct VignetteData
{
    float scale; // 16.0f
    float gamma; // 0.8f
};

ConstantBuffer<VignetteData> gVignetteData : register(b0);
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
    
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    float vignette = correct.x * correct.y * gVignetteData.scale;
    vignette = saturate(pow(vignette, gVignetteData.gamma));
    output.color.rgb *= vignette;
    
    return output;
}