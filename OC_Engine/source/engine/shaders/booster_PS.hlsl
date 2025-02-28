#include "pbr_functions.hlsli"


PixelOutput main(PixelInputType input)
{
    PixelOutput result;
    float timeMultiplier = 0.5;
    float2 scaledUV = input.uv;
    float1 t = time * timeMultiplier - floor(time * timeMultiplier);
    
    // Calculate UV coordinates for scrolling texture
    scaledUV.x = input.uv.x + t;
    
    float4 albedoScroll = aAlbedoTexture.Sample(aDefaultSampler, scaledUV).rgba;
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, input.uv).rgba;
    
    float staticIntensity = aEffectsTexture.Sample(aDefaultSampler, input.uv).b;
    float4 finalColor = lerp(albedoScroll.a, albedo.r, 0.5f);    
    finalColor *= staticIntensity;
    
    result.color.rgba = albedoScroll + (finalColor);
    return result;
}