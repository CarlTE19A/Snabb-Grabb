#include "DeferredCommon.hlsli"
#include "common.hlsli"

GBufferOutput main(DeferredVertexToPixel2 aInput) : SV_TARGET
{
    GBufferOutput output;
    float3 direction = aInput.worldPosition.xyz;
    float3 colour = skyBox.Sample(aDefaultSampler, direction).rgb;

    output.myAlbedo = float4(colour, 1.0f);
    output.myWorldPosition = 0;
    output.myNormal = 0;
    output.myMaterial = float4(0, 0, 0, 1);
    output.myEffectsAndCustom = float4(1.0f, 0.0f, 0.0f, 1.0f);
    

    return output;
}