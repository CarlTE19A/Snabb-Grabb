#include "SpriteCommon.hlsli"

float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453123);
}

float4 main(float4 color : COLOR, float2 uv : TEXCOORD, float4 position : SV_Position) : SV_TARGET0
{
    float4 texColor = spriteTexture.Sample(aDefaultSampler, uv);
    
    float gt = 30;
    
    float m = fmod(time, 1.0);
    
    bool glitch = m < 0.5;
    float t = floor(time * gt) / gt;
    float r = random(float2(t, t));
    
    if (glitch)
    {
        float2 uvGlitch = uv;
        uvGlitch.x -= r / 5.0;
        if (uv.y > r && uv.y < r + 0.01)
        {
            texColor = spriteTexture.Sample(aDefaultSampler, uvGlitch);
        }
    }
    
    return texColor;
}
