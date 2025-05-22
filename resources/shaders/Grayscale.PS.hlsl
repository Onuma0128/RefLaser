#include "Fullscreen.hlsli"

struct GrayscaleData
{
    float3 color;
    float t;
};

ConstantBuffer<GrayscaleData> gGrayscaleData : register(b0);
Texture2D<float4> gTexture : register(t0);
sampler gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    float4 texColor = gTexture.Sample(gSampler, input.texcoord);
    if (gGrayscaleData.t <= 0.0001f)
    {
        output.color = texColor;
        return output;
    }
    float grayscale = dot(texColor.rgb, gGrayscaleData.color);

    // 画面中心との距離
    float2 center = float2(0.5f, 0.5f);
    float dist = distance(input.texcoord, center);

    // 最大半径
    float maxRadius = 0.707f;

    // グレースケールがかかる半径：t が上がるほど広がる
    float radius = lerp(0.0f, maxRadius, gGrayscaleData.t);
    float delta = 0.05f; // 境界のぼかし幅

    // 中心から radius の範囲に smooth にグレースケール適用
    float grayscaleFactor = smoothstep(radius - delta, radius + delta, dist);
    grayscaleFactor = clamp(grayscaleFactor, 0.0f, 1.0f);

    // 補間
    texColor.rgb = lerp(texColor.rgb, float3(grayscale, grayscale, grayscale), (1.0f - grayscaleFactor));
    output.color = texColor;

    return output;
}