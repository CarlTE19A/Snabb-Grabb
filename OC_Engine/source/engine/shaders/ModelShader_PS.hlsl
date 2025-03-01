#include "common.hlsli"

#define DIFFUSE_MAP 0
#define NORMAL_MAP 1

Texture2D shaderTextures[2] : register(t1);

float4 main(PixelInputType input) : SV_TARGET
{
    float4 Diffuse = shaderTextures[DIFFUSE_MAP].Sample(aDefaultSampler, input.uv) * input.color;
    float3 AmbientColor = Diffuse.xyz;
    float3 DirectColor = float3(1, 0, 0);
	
    if (Diffuse.w <= 0.0f)
    {
        discard;
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    return float4(AmbientColor /*+ DirectColor*/, Diffuse.w);
}