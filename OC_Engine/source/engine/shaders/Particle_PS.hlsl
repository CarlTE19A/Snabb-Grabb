#include "ParticleCommon.hlsli"
#include "common.hlsli"

Texture2D texture_c : register(t16);
Texture2D texture_m : register(t17);
Texture2D texture_fx : register(t18);

ParticlePixelOutput main(ParticlePixelInput input)
{
    ParticlePixelOutput output;
    
    float4 textureColor = texture_c.Sample(aDefaultSampler, input.uv) /** input.color*/;
    float4 materialColor = texture_m.Sample(aDefaultSampler, input.uv);
    float4 fxColor = texture_fx.Sample(aDefaultSampler, input.uv);
    
    output.color = (textureColor + materialColor) / 3.0 * fxColor.r * input.color;
    
    //output.color.a = 1.f;
    //output.color = textureColor;
    if (output.color.a < 0.1f)
    {
        discard;
    }

    return output;
}