//#include "SpriteCommon.hlsli"
#include "common.hlsli"

PixelOutput main(PixelInput input)
{
    PixelOutput output;
    
    float4 texColor = aAlbedoTexture.Sample(aDefaultSampler, input.uv);
    float1 t = time - floor(time);
    
    float fadeLength = 0.03f;
    float amp = 0.06f;
    float frequency = 0.7f;
    float speed = 20.0f;
    
    float1 sinValue = sin(time + input.uv.x * frequency + speed);
    float1 sinValue1 = sin(time + input.uv.x);
    
    float dist = amp * sinValue1 /** input.uv.y*/ * sinValue /** input.uv.x*/;
    
    output.color = float4(texColor.rgb * input.color.rgb, texColor.a * input.color.a);
    
    //dist = dist - floor(dist);
    
    output.color.a *= smoothstep(input.value - fadeLength, input.value + fadeLength, input.uv.y + dist);
    
    
    return output;
}