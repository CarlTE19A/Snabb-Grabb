#include "BasicText.hlsli"
#include "common.hlsli"
// Constants for positioning and scaling the text

VertexOutputText main(VertexInputText input)
{
    VertexOutputText output;

    float4 vertexWorldPos = mul(modelToWorld, input.position);
    float4 vertexViewPos = mul(vertexWorldPos, worldToCameraSpace);
    float4 vertexProjectionPos = mul(vertexViewPos, cameraToProjection);
    
    output.position = vertexWorldPos;

    output.texCoord = input.texCoord;
    output.color = input.color;
    return output;
}