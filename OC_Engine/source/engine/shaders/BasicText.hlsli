//cbuffer FrameBuffer : register(b0)
//{
//    float4x4 worldToClipMatrix;
//    float4x4 cameraToProjection;
//    float4x4 yes;
//    float4 resolution; 
//}

//cbuffer ObjectBuffer : register(b1)
//{
//    float4x4 modelToWorld;
//    float3 color;
//    float alphaValue;
//}

struct VertexInputText
{
    float4 position : POSITION; 
    float4 color : COLOR;
    float2 texCoord : TEXCOORD; 
};

struct VertexOutputText
{
    float4 position : SV_POSITION; 
    float4 color : COLOR;
    float2 texCoord : TEXCOORD; 
};

struct PixelInputText
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

struct PixelOutputText
{
    float4 color : SV_TARGET0; 
};