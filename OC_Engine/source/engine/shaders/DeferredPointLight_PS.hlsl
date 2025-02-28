#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"

float4 main(DeferredVertexToPixel2 aInput) : SV_TARGET
{
    float2 uv = aInput.position.xy / resolution.zw;
    float3 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, uv.xy).rgb;
    float3 albedo = gBufferAlbedoTexture.Sample(aDefaultSampler, uv.xy).rgb;
    float3 normal = normalize(2.f * gBufferNormalTexture.Sample(aDefaultSampler, uv.xy).xyz - 1.f);
    float4 material = gBufferMaterialTexture.Sample(aDefaultSampler, uv.xy);
    
    float3 toEye = normalize(CameraPosition.xyz - worldPosition);
    
    float1 roughness = material.g;
    float1 metalness = material.b;
   
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, (float3) metalness);
    
    float3 pointLight = EvaluatePointLight(
			albedo, specularColor, normal, roughness,
			lightColourAndIntensity.rgb, lightColourAndIntensity.w, lightRange, lightPosition.xyz,
			toEye, worldPosition);
    
    return float4(pointLight, 1.f);
}