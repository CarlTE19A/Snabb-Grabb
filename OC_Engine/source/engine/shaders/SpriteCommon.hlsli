#include "common.hlsli"
//#define MAX_AMOUT_OF_SPRITES 64

Texture2D spriteTexture : register(t0);
Texture2D spriteNoiseTexture : register(t1);

cbuffer MorphBuffer : register(b11)
{
    float4 morphRadius;
    float4 morphSpeed;
    float spriteTimer;
    float3 padding;
};

struct VertexInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float value : VALUE;
};

struct InstaceBuffer
{
    float4x4 modelToWorld : INSTANCE_MODEL_TO_WORLD;
    float4 color : INSTANCE_COLOR;
    //int type;
};

struct RectVertexInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float4 morphRadius : MORPH_RADIUS;
    float4 morphSpeed : MORPH_SPEED;
    float2 uv : TEXCOORD;
    unsigned int index : SV_VertexID;
};