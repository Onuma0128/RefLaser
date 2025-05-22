#include "object3d.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    int enableLighting;
    float shininess;
    float environmentCoefficient;
};
struct DirectionalLightData
{
    float4 color;
    float3 direction;
    float intensity;
};
struct PointLightData
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
};
struct SpotLightData
{
    float4 color;
    float3 position;
    float intensity;
    float3 direction;
    float distance;
    float decay;
    float cosAngle;
    float cosFalloffStart;
};
struct Camera
{
    float3 worldPosition;
};

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLightData> gDirectionalLight : register(b1);
ConstantBuffer<PointLightData> gPointLight : register(b2);
ConstantBuffer<SpotLightData> gSpotLight : register(b3);
ConstantBuffer<Camera> gCamera : register(b4);
Texture2D<float4> gTexture : register(t0);
TextureCube<float4> gEnvironmentTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{    
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    float3 normal = normalize(input.normal);
    float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
    // DirectionalLight
    float3 lightDirectionalLight = normalize(gDirectionalLight.direction);
    float3 reflectLight = reflect(-lightDirectionalLight, normal);
    float RdotE = max(dot(reflectLight, toEye), 0.0f);
    float specularPow = pow(RdotE, gMaterial.shininess);
    // PointLight
    float3 pointLightDirecion = normalize(gPointLight.position - input.worldPosition);
    reflectLight = reflect(-pointLightDirecion, normal);
    RdotE = max(dot(reflectLight, toEye), 0.0f);
    float pointPow = pow(RdotE, gMaterial.shininess);
    // SpotLight
    float3 spotLightDirecionOnSurface = normalize(gSpotLight.position - input.worldPosition);
    reflectLight = reflect(-spotLightDirecionOnSurface, normal);
    RdotE = max(dot(reflectLight, toEye), 0.0f);
    float spotPow = pow(RdotE, gMaterial.shininess);
    
    if (gMaterial.enableLighting != 0)
    {
        // DirectionalLightの処理
        float NdotL = (dot(normal, lightDirectionalLight));
        float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float3 diffuseDirectionalLight = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        float3 specularDirectionalLight = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;
        // PointLightの処理
        NdotL = (dot(normal, pointLightDirecion));
        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float distance = length(gPointLight.position - input.worldPosition);
        float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
        float3 diffusePointLight = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
        float3 specularPointLight = gPointLight.color.rgb * gPointLight.intensity * pointPow * factor;
        // SpotLightの処理
        NdotL = (dot(normal, spotLightDirecionOnSurface));
        cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
        float cosAngle = dot(spotLightDirecionOnSurface, gSpotLight.direction);
        float falloffFactor = saturate((cosAngle - gSpotLight.cosAngle) / (gSpotLight.cosFalloffStart - gSpotLight.cosAngle));
        float spotDistance = length(gSpotLight.position - input.worldPosition);
        float spotDistanceFactor = pow(saturate(-spotDistance / gSpotLight.distance + 1.0f), gSpotLight.decay);
        float3 diffuseSpotLight = gMaterial.color.rgb * textureColor.rgb * gSpotLight.color.rgb * cos * gSpotLight.intensity * spotDistanceFactor * falloffFactor;
        float3 specularSpotLight = gSpotLight.color.rgb * gSpotLight.intensity * spotPow * spotDistanceFactor * falloffFactor;
        // 環境マップの処理
        float3 cameraToPosition = normalize(input.worldPosition - gCamera.worldPosition);
        float3 reflectedVector = reflect(cameraToPosition, normalize(input.normal));
        float4 environmentColor = gEnvironmentTexture.Sample(gSampler, reflectedVector);
        environmentColor.rgb *= gMaterial.environmentCoefficient;
        // ライトの処理を合算
        output.color.rgb =
        diffuseDirectionalLight + specularDirectionalLight +
        diffusePointLight + specularPointLight +
        diffuseSpotLight + specularSpotLight +
        environmentColor.rgb;
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }

    if (textureColor.a == 0.0f || textureColor.a <= 0.5f || output.color.a == 0.0f)
    {
        discard;
    }
    
    return output;
}