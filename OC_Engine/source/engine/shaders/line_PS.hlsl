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

float4 main(PixelInputTypeLine input) : SV_TARGET
{
    float4 textureColor = input.color;
    return textureColor;
}