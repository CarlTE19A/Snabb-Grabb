#include "SpriteCommon.hlsli"

float4 main(float4 color : COLOR, float2 uv : TEXCOORD, float4 position : SV_Position) : SV_TARGET0
{
    float4 texColor = spriteTexture.Sample(aDefaultSampler, uv);
    float4 noiseColor = spriteNoiseTexture.Sample(aDefaultSampler, uv);
    float timer = spriteTimer;
    
    float2 lerpCol = lerp(uv, noiseColor.rb, 0.5f);
    
    
    //gradient scroll from the bottom uv.y to the top uv.y color based on uv and time
    float gradient = saturate(lerpCol.y - timer);
    
    if (gradient > 0.04 && gradient < 0.05 && texColor.a > 0.01f)
    {
        texColor = float4(1.f, 0.098f, 0.074f, 1.0f); // Red color
    }
    else if (gradient > 0.05)
    {
        texColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    return texColor;
}