#include "DeferredCommon.hlsli"
#include "common.hlsli"

DeferredVertexToPixel2 main(DeferredVertexInput aInput)
{
    DeferredVertexToPixel2 output;
    
    float4 vertexObjectPos = aInput.position;
    float4 pos = float4(aInput.position.xyz, 0.0f);
    float4 vertexViewPos = mul(worldToCameraSpace, pos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
    output.worldPosition = vertexObjectPos;
    output.position = vertexProjectionPos;
    output.position.z = output.position.w;
	
    return output;
}
