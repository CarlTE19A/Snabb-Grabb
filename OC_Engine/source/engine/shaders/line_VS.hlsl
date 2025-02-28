#include "common.hlsli"
struct VertexInputTypeLine
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputTypeLine
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInputTypeLine main(VertexInputTypeLine input)
{
    PixelInputTypeLine output;

    float4 vertexClipPos = mul(worldToCameraSpace, input.position);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexClipPos);
    
    output.position = vertexProjectionPos;
    output.color = input.color;
    
    return output;
}