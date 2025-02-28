#include "DeferredCommon.hlsli"
#include "common.hlsli"

PixelOutput main(FullscreenVertexToPixel input)
{
    PixelOutput returnValue;
	returnValue.color.rgb = FullscreenTexture1.Sample(aDefaultSampler, input.UV).rgb;
	returnValue.color.a = 1.0f;
	return returnValue;
}