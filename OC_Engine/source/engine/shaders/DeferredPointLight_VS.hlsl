#include "DeferredCommon.hlsli"
#include "common.hlsli"

//DeferredVertexToPixel main(DeferredVertexInput aInput)
//{
//    DeferredVertexToPixel output;
//    //float4 vertexObjectPos = aInput.position;
    
//    float4x4 objectToWorld = float4x4(
//        lightRange, 0, 0, 0,
//        0, lightRange, 0, 0,
//        0, 0, lightRange, 0,
//        lightPosition.x, lightPosition.y, lightPosition.z, 1
//        );

//    objectToWorld = transpose(objectToWorld);

//    //float4 vertexWorldPos = mul(objectToWorld, vertexObjectPos);
//    //    //const float4 pos = aInput.position;// * lightPosAndRange.w;
//    //    //output.position = pos + float4(lightPosAndRange.xyz, 0.0f);
//    //float4x4 worldToClipSpaceMatrix = worldToClipMatrix * cameraToProjection;
//    //output.position = mul(worldToClipSpaceMatrix, vertexWorldPos);
//    //return output;
    
//    //DeferredVertexToPixel output;

//    float4 vertexObjectPos = aInput.position;
//    float4 vertexWorldPos = mul(objectToWorld, vertexObjectPos);
//    float4 vertexViewPos = mul(worldToClipMatrix, vertexWorldPos);
//    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
	
//    output.position = vertexProjectionPos;
	
//    return output;
//}

DeferredVertexToPixel2 main(DeferredVertexInput aInput)
{
    DeferredVertexToPixel2 output;
    
    float4 vertexObjectPos = aInput.position;
    float4 vertexWorldPos = mul(modelToWorld, vertexObjectPos);
    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
	
    output.position = vertexProjectionPos;
    output.worldPosition = vertexWorldPos;
	
    return output;
}