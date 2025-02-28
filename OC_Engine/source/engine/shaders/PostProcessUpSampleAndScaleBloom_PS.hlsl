#include "DeferredCommon.hlsli"
#include "common.hlsli"

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput returnValue;
    float2 pixelOffset = float2(ddx(input.UV.x), ddy(input.UV.y));
    float3 p00 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(-1.0f, -1.0f)).rgb;
    float3 p01 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(-1.0f, 1.0f)).rgb;
    float3 p10 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(1.0f, -1.0f)).rgb;
    float3 p11 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(1.0f, 1.0f)).rgb;
    returnValue.color.rgb = 0.25f * (p00 + p01 + p10 + p11);
    returnValue.color.a = BloomBlending;
    return returnValue;
}