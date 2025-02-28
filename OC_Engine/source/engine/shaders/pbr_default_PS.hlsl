#include "pbr_functions.hlsli"

float3 expandNormal(float3 normalTexture)
{
    float3 normal = 2.0f * normalTexture - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    return normalize(normal);
}
// nya packningen för texturer
PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    
    float2 scaledUV = input.uv;
    float3 toEye = normalize(CameraPosition.xyz - input.worldPosition.xyz);
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, scaledUV).rgba;
    
    if (albedo.a <= 0.5f)
    {
        discard;
        result.color = float4(0.f, 0.f, 0.f, 0.f);
        return result;
    }
    
    float3 normal = aNormalTexture.Sample(aDefaultSampler, scaledUV).xyz;
        
    normal = expandNormal(normal);
    
    float3x3 TBN = float3x3(input.tangent.xyz, -input.binormal.xyz, input.normal.xyz);
    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));
    
    float3 material = aMaterialTexture.Sample(aDefaultSampler, scaledUV).rgb;
    
    float ambientOcclusion = material.r;
    float1 roughness = material.g;
    float1 metalness = material.b;
    
    float3 effects = aEffectsTexture.Sample(aDefaultSampler, scaledUV).rgb;
    float emissive = effects.r;
    
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, (float3) metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, (float3) 1 - (float3) metalness);
    
    float3 ambiance = EvaluateAmbiance(
		aEnvironmentTexture, pixelNormal, input.normal.xyz,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 dir = dLightDir.xyz;
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		dLightColorAndIntensity.xyz, dir, toEye.xyz
	);
        
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambiance + directionalLight + emissiveAlbedo;
    
    result.color = float4(radiance.rgb, 0.5f);
    result.color.a = albedo.a;
    return result;
}