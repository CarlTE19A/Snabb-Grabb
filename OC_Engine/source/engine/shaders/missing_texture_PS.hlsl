#include "common.hlsli"

PixelOutput main(PixelInputType input)
{
	PixelOutput result;
    float4 vertexObjectNormal = float4(input.normal, 0.0);
    float4 vertexWorldNormal = mul(modelToWorld, vertexObjectNormal);

    result.color = float4(0, 0, 1, 1);

	return result;
}