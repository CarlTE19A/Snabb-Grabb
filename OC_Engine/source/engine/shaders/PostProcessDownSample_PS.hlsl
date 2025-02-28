#include "DeferredCommon.hlsli"
#include "common.hlsli"

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput returnValue;
// ddx and ddy measures how much a parameter changes in x and y-direction per pixel
// could have used
    float2 pixelOffset = float2(ddx(input.UV.x), ddy(input.UV.y));
// Could have done one sample in the middle
// But that results in some artifacts. This pattern gives a much smoother result
    float3 p00 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(-0.5f, -0.5f)).rgb;
    float3 p01 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(-0.5f, 0.5f)).rgb;
    float3 p10 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(0.5f, -0.5f)).rgb;
    float3 p11 = FullscreenTexture1.Sample(aDefaultSampler, input.UV + pixelOffset * float2(0.5f, 0.5f)).rgb;
    returnValue.color.rgb = 0.25f * (p00 + p01 + p10 + p11);
    returnValue.color.a = 1.0f;
    return returnValue;
}