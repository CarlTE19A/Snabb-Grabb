#include "common.hlsli"

PixelInputType main(VertexInputType input)
{
    PixelInputType result;

    float4 vertexObjectPos = input.position;
    //float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexWorldPos = mul(input.transform, vertexObjectPos);
    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);

    float3x3 toWorldRotation = (float3x3) input.transform;
    float3 vertexWorldNormal = mul(toWorldRotation, input.normal);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.binormal);
    float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);
	
    result.position = vertexProjectionPos;
    result.worldPosition = vertexWorldPos;
    

    result.uv = input.uv;

    result.normal = vertexWorldNormal;
    result.binormal = vertexWorldBinormal;
    result.tangent = vertexWorldTangent;
    result.color = input.color;
	
    return result;
}