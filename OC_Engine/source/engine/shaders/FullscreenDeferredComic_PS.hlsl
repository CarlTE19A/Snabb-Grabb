#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"

float ConstantBiasScale(float inputValue)
{
    float bias = 1.0; //fråga tyra om värden
    float scale = 0.5;
    return (inputValue + bias) * scale;
}

float RemapValueRange(float inputValue, float iLow, float iHigh, float tLow, float tHigh)
{
    return ((inputValue - iLow) / (iHigh - iLow)) * (tHigh - tLow) + tLow;
}

#define SAMPLES_COUNT 64 
#define SAMPLES_COUNT_DIV_2 32  
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)  

PixelOutput main(FullscreenVertexToPixel input)
{
    float2 uv = input.UV;
    float2 gUV = input.Position.xy / resolution.zw;
    float4 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, uv).rgba;
    float4 vertexViewPos = mul(worldToCameraSpace, worldPosition);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos); //this!

    float4 color = FullscreenTexture1.Sample(aDefaultSampler, uv).rgba;
    
    float4 texperlin = perlinTexture.Sample(aDefaultSampler, uv);
    
    PixelOutput result;
    
    
        //do cool stuff so it looks like borderlands 3 natta
    //ok will do my best c:
    
    
        //paper
    float3 emissivepaper = lerp(color.rgb, float3(0, 0, 0), color.rgb);
    

    
    //float4 worldPos = CameraPosition - worldPosition;
    float4 worldPos = worldPosition;
    //float4 worldPos = vertexProjectionPos;
    
    //perlin
    //float3 threeHundred = (300, 300, 300);
    float1 constbscale = ConstantBiasScale(texperlin.r); // do i have to do this?
    
    float1 valueper = constbscale * .002f;
    
    float2 uvper = uv + valueper;
    float4 materialper = FullscreenTexture1.Sample(aDefaultSampler, uvper);
    
    
    
    float3 emissiveper = emissivepaper * materialper.rgb;
    
    
    //lineart
    float4 campos = CameraPosition;

    float1 depth = length(worldPos - campos);

    float1 clampedWorldpos = clamp(depth, 100.f, 600.f);
    //float1 remap = RemapValueRange(clampedWorldpos, 100.f, 1080.f, 0.25f, 1.5f); // tweak this value to get the desired effect
    //float1 remap = RemapValueRange(clampedWorldpos, 10.f, 10080.f, 0.06f, 0.01f); // tweak this value to get the desired effect
    float1 remap = RemapValueRange(clampedWorldpos, 1080.f, 100.f, 0.005f, 0.7f); // tweak this value to get the desired effect
    
    //remap = .5f;

    
    //float4 d = aDepthTexture.Sample(aDefaultSampler, uv);

    //depth = RemapValueRange(depth, 0, 1000.f, 0, 1.f);
    
    //if (1/depth <= 0.001f )
    //{
    //    result.color.rgba = float4(1, 0, 0, 1);
    //}
    //result.color.rgba = float4(d.rgb, 1);
    
    
    
    float1 oneDdepth = (1 / depth);
    
    ////upper
    float2 val = (oneDdepth * float2(0, 1)) * remap + uv;
    float4 cal = gBufferPositionTexture.Sample(aDefaultSampler, val).rgba;
    float1 valU = (depth - length(cal - campos)); //abs?
        
    if (val.y > 1.f)
        valU = 0.f;
    
    float2 val1 = (oneDdepth * float2(0, -1)) * remap + uv;
    float4 cal1 = gBufferPositionTexture.Sample(aDefaultSampler, val1).rgba;
    float1 valU1 = (depth - length(cal1 - campos));
    
    if (val1.y < 0.f)
        valU1 = 0.f;
    
    valU = valU + valU1;

    float2 val2 = (oneDdepth * float2(1, 0)) * remap + uv;
    float4 cal2 = gBufferPositionTexture.Sample(aDefaultSampler, val2).rgba;
    float1 valU2 = (depth - length(cal2 - campos));
    
    if (val2.x > 1.f)
        valU2 = 0.f;
    
    float2 val3 = (oneDdepth * float2(-1, 0)) * remap + uv;
    float4 cal3 = gBufferPositionTexture.Sample(aDefaultSampler, val3).rgba;
    float1 valU3 = (depth - length(cal3 - campos));
    
    if (val3.x < 0.f)
        valU3 = 0.f;
    
    valU2 = valU2 + valU3;
    
    
    valU = valU + valU2;
    valU = pow(valU, 4);
    valU *= valueper;
    
    //end of cool stuff :c
    
    if (valU > 0.01f)
    {
        result.color.rgb = float3(0, 0, 0);
    }
    else
    {
        result.color.rgb = color.rgb;
    }
    
    result.color.a = 1.f;
    //result.color.rgba = float4(0, 0, 1, 1);
    return result;
}
