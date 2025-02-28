#include "SpriteCommon.hlsli"

cbuffer Parameters : register(b0)
{
    row_major float4x4 MatrixTransform;
};

void main(inout float4 color : COLOR0, inout float2 texCoord : TEXCOORD0, inout float4 position : SV_Position, in uint vid : SV_VertexID)
{
   // color.rgb = (float3) id / 20;
    uint id = vid % 4;
    
    float1 timer = time * morphSpeed[id];
    
    float angle = timer * 2 * 3.1415;
    
    float x = morphRadius[id] * cos(angle);
    float y = morphRadius[id] * sin(angle);
    
    
    position.xy += float2(x, y);
    
    position = mul(position, MatrixTransform);
}

//PixelInput main(RectVertexInput input, InstaceBuffer instance)
//{
//    PixelInput output;
//    if (instance.color.a > 0.1f)
//    {
//        float4 vertexWorldPos = mul(instance.modelToWorld, input.position);
//        float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
//        float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);

//        float2 pos = vertexWorldPos.xy / resolution.zw;
        
//        output.color = float4(instance.color);

//        output.uv = input.uv;
        
//        //output.value = input.value;
        
//        output.position = float4(pos.xy, vertexWorldPos.zw / resolution.zw);
    
//        return output;
//    }

//    float4 temppos = input.position;
//    temppos += input.morphRadius[input.index];
//    //sin curve for rotation 
    
    
//    float4 vertexWorldPos = mul(instance.modelToWorld, temppos);
//    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
//    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);

//    float2 pos = vertexWorldPos.xy / resolution.zw;
    
//    //vertexClipPos.y = resolution.w;
    
//    output.position = float4(pos.xy /*+ offset.xy*/, vertexWorldPos.zw / resolution.zw);

//    output.color = float4(instance.color);
//    output.uv = input.uv;
//    //output.value = input.value;
    
//    return output;
//}