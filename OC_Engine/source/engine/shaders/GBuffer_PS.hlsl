#include "Common.hlsli"

GBufferOutput main(PixelInputType input)
{
    float2 scaledUV = input.uv;
    float3 albedo = aAlbedoTexture.Sample(aDefaultSampler, scaledUV).rgb;
    float3 normal = aNormalTexture.Sample(aDefaultSampler, scaledUV).xyz;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);
    float3x3 TBN = float3x3(
        normalize(input.tangent.xyz),
        normalize(-input.binormal.xyz),
        normalize(input.normal.xyz));
    TBN = transpose(TBN);
    
    float emissive = aEffectsTexture.Sample(aDefaultSampler, scaledUV).r;
    float3 pixelNormal = normalize(mul(TBN, normal));
    float4 material = aMaterialTexture.Sample(aDefaultSampler, scaledUV);
    GBufferOutput output;
    output.myWorldPosition = input.worldPosition;
    output.myAlbedo = float4(albedo, highlight);
    output.myNormal = float4(0.5f + 0.5f * pixelNormal, 1.0f);
    output.myMaterial = float4(material);
    
    output.myEffectsAndCustom = float4(emissive, input.normal); // gba are unused, put whatever data you want here!
    return output;
}

//Texture2D aAlbedoTexture : register(t1);
//Texture2D aNormalTexture : register(t2);
//Texture2D aMaterialTexture : register(t3);
//Texture2D aEffectsTexture : register(t4);
