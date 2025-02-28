#include "Common.hlsli"

GBufferOutput main(PixelInputType input)
{
    float2 scaledUV = input.uv;
    
    GBufferOutput output;
    output.myWorldPosition = input.worldPosition;
    output.myAlbedo = float4(0.0f, 1.0f, 1.0f, 0.0f);
    output.myNormal = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.myMaterial = float4(1.0f, 1.0f, 1.0f, 1.0f);
    output.myEffectsAndCustom = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return output;
}