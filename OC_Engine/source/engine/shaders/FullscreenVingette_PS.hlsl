#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"
//Inner radius
#define inner .5
//Outer radius
#define outer 1.2
//Vignette strength/intensity
#define strength .8
//Vignette roundness, higher = smoother, lower = sharper
#define curvature .5

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput result;
    const float width = 300.0;
    float2 uv = input.Position.xy / resolution.zw;
    float4 tex = float4(1, 1, 1, 1);    
    
    //Calculate edge curvature
    float2 curve = pow(abs(uv * 2. - 1.f), float2(1.f / curvature, 1.f / curvature));
    //Compute distance to edge
    float edge = pow(length(curve), curvature);
    //Compute vignette gradient and intensity
    float vignette = 1.f - strength * smoothstep(inner, outer, edge);
    
    //Add vignette to the resulting texture
    tex.rgb *= vignette;
    
    float1 x = (1 - vignette) * 1 + vignette * 0;
    
    result.color.rgb = float3(0,0,0);
    
    result.color.a = x;
    
    return result;
}