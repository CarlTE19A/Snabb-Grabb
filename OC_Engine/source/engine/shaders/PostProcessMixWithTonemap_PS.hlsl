#include "DeferredCommon.hlsli"
#include "common.hlsli"

// Origin: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}


float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput returnValue;
    float3 color = FullscreenTexture1.Sample(aDefaultSampler, input.UV.xy).rgb;

    float3 newColor = exp2(Exposure) * color;
    
    float luminance = dot(float3(0.2126, 0.7152, 0.0722), newColor);
    newColor = luminance + Saturation * (newColor - luminance);
    
    newColor = 0.18f * pow(newColor / 0.18f, Contrast); // 0.18 is perceived as 50% grey. Keep it constant
    
    newColor = max(0.0f, newColor - Blackpoint);
    
    newColor = newColor * Tint;
    
    returnValue.color.rgb = tonemap_s_gamut3_cine(newColor);

    returnValue.color.a = 1.0f;
    return returnValue;
}