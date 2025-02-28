#include "VolumetricsCommon.hlsli"

float sampleDensityFog(float3 position, Texture3D noiseTexture, float3 offsetPos);

FullscreenPixelOutput main(FullscreenVertexToPixel input)
{
    FullscreenPixelOutput output;
    float4 col = FullscreenTexture1.Sample(aDefaultSampler, input.UV).rgba;
    
    float4 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, input.UV).xyzw;
    float3 rayOrigin = CameraPosition.xyz;
    float3 rayDir = normalize(input.viewVector);
   
    //float3 worldPos = rayOrigin + CameraForward.xyz * FarPlane;
    //float3 rayDir = normalize(worldPos.xyz - rayOrigin);
    
    // Compute depth
    float depth = GetLinDepth(worldPosition) * length(rayDir);
    
    // Calculate ray-box intersection
    float2 rayBoxInfo = rayBoxDist(BoundsMin.xyz, BoundsMax.xyz, rayOrigin, 1/rayDir);
    float dstToBox = rayBoxInfo.x;
    float dstInsideBox = rayBoxInfo.y;
    
     // Ensure ray marching starts from a fixed height
    //float startHeight = max(MinViewHeight, dstToBox);
    // Entry point into the cloud volume
    float3 entryPoint = rayOrigin + rayDir * dstToBox /** MinViewHeight*/;
    
    float randomOffset = rand_1_05(input.UV * 3.0f);
    // Perform ray marching
    float dstTravelled = randomOffset;
    float stepSize = dstInsideBox / numSteps;
    //float dstLimit = min(depth - dstToBox, dstInsideBox);
    
    float3 animatedFogPos = float3(CloudOffset.x, CloudOffset.y /*+ time*/, CloudOffset.z) * 0.01;
    // Phase function makes clouds brighter around sun
    float cosAngle = dot(rayDir, dirLightPosition.xyz);
    float phaseVal = phase(cosAngle);
    
    float transmittance = 1;
    float3 lightEnergy = 0;
    
    [loop]
    while (dstTravelled < dstInsideBox)
    {
        float3 rayPos = entryPoint + rayDir * dstTravelled;
        float density = sampleDensityFog(rayPos, perlinNoise, animatedFogPos);
        //fix a trim
        if (density > 0)
        {

            float lightTransmittance = lightmarch(rayPos, cosAngle, perlinNoise, animatedFogPos);
            lightEnergy += density * stepSize * transmittance * lightTransmittance * phaseVal;
            transmittance *= exp(-density * stepSize * lightAbsorptionThroughCloud);
                    
                        // Early exit
            if (transmittance < 0.01)
            {
                break;
            }
        }
                    
        dstTravelled += stepSize;
    }
    
    float dstFog = 1 - exp(-max(0, depth) * 8 * .0001);
    col = col * (1 - dstFog);
    float4 fogCol = float4(lightEnergy * float3(0.91, 0.898, 0.98), 0)/* * (1 - dstFog)*/;
    output.color = col * transmittance + fogCol;
    output.color.a = 0;
    return output;
}

 //You stupid


float sampleDensityFog(float3 position, Texture3D noiseTexture, float3 offsetPos)
{
    //float3 size = BoundsMax.xyz - BoundsMin.xyz;
    //float3 uvw = (size * 0.5f + position) * CloudScale * 0.001;
    //float3 shapeSamplePos = uvw + offsetPos;
    //float density = noiseTexture.Sample(aDefaultSampler, shapeSamplePos).r;
    //density = (density - DensityThreshold) * DensityMultiplier;
    //return density;
    
    
    ////for x
    //float boxXlength = (BoundsMax.x + abs(BoundsMin.x));
    //float worldPosInBoxSpaceX = position.x + abs(BoundsMin.x);
    //float normalizedXposition = worldPosInBoxSpaceX / boxXlength;
    //float pixelCoordX = normalizedXposition * wTexturesize.x;
    ////for y
    //float boxYlength = (BoundsMax.y + abs(BoundsMin.y));
    //float worldPosInBoxSpaceY = position.y + abs(BoundsMin.y);
    //float normalizedYposition = worldPosInBoxSpaceY / boxYlength;
    //float pixelCoordY = normalizedYposition * wTexturesize.y;
    ////for z
    //float boxZlength = (BoundsMax.z + abs(BoundsMin.z));
    //float worldPosInBoxSpaceZ = position.z + abs(BoundsMin.z);
    //float normalizedZposition = worldPosInBoxSpaceZ / boxZlength;
    //float pixelCoordZ = normalizedZposition * wTexturesize.z;
    
    //float3 newPos = (pixelCoordX, pixelCoordY, pixelCoordZ);
    float3 wTexturesize;
    noiseTexture.GetDimensions(wTexturesize.x, wTexturesize.y, wTexturesize.z);
    //float3 boxLength = BoundsMax.xyz + abs(BoundsMin.xyz);
    //float3 worldPosInBox = position + abs(BoundsMin.xyz);
    //float3 normalizedPosition = worldPosInBox / boxLength;
    //float3 pixelCoordPos = normalizedPosition * wTexturesize;
    
    ////float3 size = BoundsMax.xyz - BoundsMin.xyz;
    
    //float3 uvw = (/*size * 0.5 +*/ pixelCoordPos) * CloudScale * 0.001 + offsetPos;
    //float shape = noiseTexture.Sample(aDefaultSampler, uvw).r; //should add shapenoise??
    //float density = (shape - DensityThreshold) * DensityMultiplier;
    //return density;
    
//    // Calculate the size of the fog volume
//    float3 boxLength = BoundsMax.xyz - BoundsMin.xyz;

//// Calculate the normalized position within the fog volume
//    float3 normalizedPosition = (position - BoundsMin.xyz) / boxLength;

//// Calculate the position in texture space
//    float3 pixelCoordPos = normalizedPosition * wTexturesize;

//// Calculate the UVW coordinates for sampling the noise texture
//    float3 uvw = (pixelCoordPos * CloudScale * 0.001) + offsetPos;

//// Sample the noise texture to obtain the fog shape
//    float shape = noiseTexture.SampleGrad(aDefaultSampler, uvw, ddx(uvw), ddy(uvw)).r;

//// Calculate fog density based on the sampled shape
//    float density = (shape - DensityThreshold) * DensityMultiplier;

//    return density;
    
    float3 size = BoundsMax.xyz - BoundsMin.xyz;
    float3 uvw = (size * 0.5f + position) * CloudScale * 0.001;
    float3 shapeSamplePos = uvw + offsetPos;
    float density = noiseTexture.Sample(aDefaultSampler, shapeSamplePos).r;
    density = (density /*- DensityThreshold*/) * DensityMultiplier;
    return density;
}