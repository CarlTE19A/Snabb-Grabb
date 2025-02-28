#include "common.hlsli"

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	float4 vertexObjectPos = input.position;
	float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexClipPos = mul(worldToCameraSpace, vertexWorldPos);
	
	output.color = float4(input.color.xyz, 1.0f);
    output.normal = input.normal;
	output.position = vertexClipPos;
    output.uv = input.uv;
	output.binormal = input.binormal;
	output.tangent = input.tangent;
	output.worldPosition = vertexWorldPos;

	return output;
}
