#include "pbr_functions.hlsli"


PixelOutput main(PixelInputType input)
{
    PixelOutput result;
     
    float2 scaledUV = input.uv;
    float1 t = time - floor(time);
     
    scaledUV.x = input.uv.x + t;
    float4 albedo = aAlbedoTexture.Sample(aDefaultSampler, scaledUV).rgba;
     
    float1 mask = aEffectsTexture.Sample(aDefaultSampler, input.uv).r;
     
    result.color = albedo/* * mask*/;
   // result.color.a = 1;
    
    //float4 mask2 = mask * mask;
    //result.color.rgba *= mask;

     
    return result;
}
