Texture2D fontTexture : register(t1); // Font texture

SamplerState fontSampler : register(s0); // Texture sampler

#include "BasicText.hlsli"
#include "common.hlsli"

float4 main(VertexOutputText input) : SV_TARGET
{
    float4 tempColor = fontTexture.Sample(fontSampler, input.texCoord) * float4(color.xyz, 1.f);
    
    return tempColor;
}