#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
sampler gSampler : register(s0);

struct PixelShaderOutpot
{
    float4 color : SV_TARGET0;
};

static const float PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};
static const float2 kIndex5x5[5][5] =
{
    { { -1.0f, -1.0f }, { -0.5f, -1.0f }, { 0.0f, -1.0f }, { 0.5f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, -0.5f }, { -0.5f, -0.5f }, { 0.0f, -0.5f }, { 0.5f, -0.5f }, { 1.0f, -0.5f } },
    { { -1.0f, 0.0f }, { -0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.5f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 0.5f }, { -0.5f, 0.5f }, { 0.0f, 0.5f }, { 0.5f, 0.5f }, { 1.0f, 0.5f } },
    { { -1.0f, 1.0f }, { -0.5f, 1.0f }, { 0.0f, 1.0f }, { 0.5f, 1.0f }, { 1.0f, 1.0f } }
};

static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
};
static const float kKernel5x5[5][5] =
{
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
};

PixelShaderOutpot main(VertexShaderOutput input)
{
    float weight = 0.0f;
    float kernel3x3[3][3];
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
            weight += kernel3x3[x][y];
        }
    }
    
    int width, height;
    gTexture.GetDimensions(width, height);
    float2 uvStepSize = float2(rcp((float) width), rcp((float) height));
    
    PixelShaderOutpot output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    
    for (x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel3x3[x][y];
        }
    }
    output.color.rgb *= rcp(weight);
    
    return output;
}