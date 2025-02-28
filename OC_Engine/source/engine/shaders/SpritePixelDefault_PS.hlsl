#include "SpriteCommon.hlsli"

float4 main(float4 color : COLOR, float2 uv : TEXCOORD, float4 position : SV_Position) : SV_TARGET0
{
    float4 texColor = spriteTexture.Sample(aDefaultSampler, uv);

    //float index = float(pxID % 4);
    
    //texColor.r = 1.0f - (index / 4.0f);
    //float4 col;
    //int index = id;

    //if (index == 0)
    //{
    //    col = float4(1.0f, 0.0f, 0.0f, 1.0f);
    //}
    //else if (index == 1)
    //{
    //    col = float4(0.0f, 1.0f, 0.0f, 1.0f);
    //}
    //else if (index == 2)
    //{
    //    col = float4(0.0f, 0.0f, 1.0f, 1.0f);
    //}
    //else if (index == 3)
    //{
    //    col = float4(1.0f, 1.0f, 1.0f, 1.0f);
    //}
    //else
    //{
    //    col = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //}
     
    
    
    return texColor;
}

//PixelOutput main(PixelInput input)
//{
//    PixelOutput output;
        
//    float4 texColor = spriteTexture.Sample(aDefaultSampler, input.uv);
    
//    if (texColor.a < 0.018f)
//        discard;
//            output.color = float4(texColor.rgb * input.color.rgb /*color.rgb*/, texColor.a * input.color.a /*texColor.a * alphaValue*/);
    
//    //switch (input.type)
//    //{
//        //case 0: // default
//            //output.color = float4(texColor.rgb * input.color.rgb /*color.rgb*/, texColor.a * input.color.a /*texColor.a * alphaValue*/);
//            //break;
//        //case 1: // cooldown
        
//            //break;
//        //case 2: // flow
//            //float1 t = time - floor(time);
//            //float fadeLength = 0.03f;
//            //float amp = 0.06f;
//            //float frequency = 0.7f;
//            //float speed = 20.0f;
//            //float1 sinValue = sin(time + input.uv.x * frequency + speed);
//            //float1 sinValue1 = sin(time + input.uv.x);
    
//            //float dist = amp * sinValue1 * sinValue ;
    
//            //output.color = float4(texColor.rgb * input.color.rgb, texColor.a * input.color.a);
    
//            //output.color.a *= smoothstep(input.value - fadeLength, input.value + fadeLength, input.uv.y + dist);
//            //break;
//        //case 3:
        
//        //default:
//            //break;
//    //}
    
//    return output;
//}