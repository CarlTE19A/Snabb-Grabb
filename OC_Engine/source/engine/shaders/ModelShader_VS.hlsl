#include "common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    float4 vertexObjectPos = input.position;
    float4 vertexWorldPos = mul(input.transform, vertexObjectPos);
    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
	
    float4 vertexObjectNormal = float4(input.normal, 0.0);

    float4 vertexObjectTangent = float4(input.tangent, 0.0);
    
    float4 vertexObjectBitangent = float4(input.binormal, 0.0f);

    output.normal = mul(input.transform, vertexObjectNormal).xyz;

    output.tangent = mul(input.transform, vertexObjectTangent).xyz;
    
    output.binormal = mul(input.transform, vertexObjectBitangent).xyz;
	
    //output.binormal = mul(modelToWorld, float4(input.binormal, 0)).xyz;
    //output.normal = mul(modelToWorld, float4(input.normal, 0)).xyz;
    //output.tangent = mul(modelToWorld, float4(input.tangent, 0)).xyz;
	
    output.uv = input.uv;
    output.position = vertexProjectionPos;
    output.worldPosition = vertexWorldPos;
    output.color = input.color;
	
    return output;
}