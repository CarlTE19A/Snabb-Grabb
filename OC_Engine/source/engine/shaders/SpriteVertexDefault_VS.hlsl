#include "SpriteCommon.hlsli"

PixelInput main(VertexInput input, InstaceBuffer instance)
{
    PixelInput output;
    if (instance.color.a > 0.1f)
    {
        float4 vertexWorldPos = mul(instance.modelToWorld, input.position);
        float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
        float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);

        float2 pos = vertexWorldPos.xy / resolution.zw;
        
        output.color = float4(instance.color);

        output.uv = input.uv;
        
        output.value = input.value;
        
        output.position = float4(pos.xy, vertexWorldPos.zw / resolution.zw);
    
        return output;
    }

    float4 vertexWorldPos = mul(instance.modelToWorld, input.position);
    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);

    float2 pos = vertexWorldPos.xy / resolution.zw;
    
    //vertexClipPos.y = resolution.w;
    
    output.position = float4(pos.xy /*+ offset.xy*/, vertexWorldPos.zw / resolution.zw);

    output.color = float4(instance.color);
    output.uv = input.uv;
    output.value = input.value;
    
    return output;
}