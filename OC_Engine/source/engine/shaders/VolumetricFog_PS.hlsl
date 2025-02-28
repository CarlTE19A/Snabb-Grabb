#include "VolumetricsCommon.hlsli"

FullscreenPixelOutput main(VertexToPixelCube input)
{
    FullscreenPixelOutput output;
    
    float4 worldPosition = input.WorldPosition;
    float3 rayOrigin = CameraPosition.xyz;
    float3 rayDir = normalize(worldPosition.xyz - rayOrigin);
    
   //float2 uv = input.Position.xy / resolution.zw;
   //float3 color = FullscreenTexture1.Sample(aDefaultSampler, uv).rgb;

    // Calculate ray-box intersection
    float2 rayBoxInfo = rayBoxDist(BoundsMin.xyz, BoundsMax.xyz, rayOrigin, rayDir);
    float dstToBox = rayBoxInfo.x;
    float dstInsideBox = rayBoxInfo.y;
    
    // Entry point into the cloud volume
    float3 entryPoint = rayOrigin + rayDir * dstToBox;
    float randomOffset = rand_1_05(input.UV * 3.0f);
    // Perform ray marching
    float dstTravelled = randomOffset;
    float stepSize = dstInsideBox / numSteps;
    
    // Phase function makes clouds brighter around sun
    float3 dirToLight = normalize(dirLightPosition.xyz - entryPoint);
    float cosAngle = dot(rayDir, dirToLight);
    float phaseVal = PhaseFunction(cosAngle, phaseParams.x, phaseParams.y, phaseParams.z, phaseParams.w);
    //float phaseVal = phase(cosAngle);
    float3 animatedFogPos = float3(CloudOffset.x, CloudOffset.y + time * 3.f, CloudOffset.z) * 0.01;
    float transmittance = 1;
    float3 lightEnergy = 0;
    [loop]
    while (dstTravelled < dstInsideBox)
    {
        float3 rayPos = entryPoint + rayDir * dstTravelled;
        float density = sampleDensity(rayPos, perlinNoise, animatedFogPos);
        density = TrimCloud(rayPos, density, density);
        //fix a trim
        if (density > 0)
        {

            float lightTransmittance = lightmarch(rayPos, cosAngle, perlinNoise, animatedFogPos);
            lightEnergy += density * stepSize * transmittance * lightTransmittance * phaseVal;
            transmittance *= beer(density * stepSize * lightAbsorptionThroughCloud);
                    
                        // Early exit
            if (transmittance < 0.01)
            {
                break;
            }
        }
        dstTravelled += stepSize;
    }
    
    float depth = GetLogDepth(worldPosition) * length(rayDir);
    float dstFog = 1 - exp(-max(0, depth) * 8 * .0001);
    //float4 cloudCol = float4(lightEnergy * dLightColorAndIntensity.xyz, 0) * (1 - transmittance);
    float4 fogCol = float4(lightEnergy * float3(0.91, 0.898, 0.98), 0) * (1 - dstFog);
    output.color.rgb = /*color * (1 - dstFog) +*/fogCol.rgb;
    output.color.a = 1 - transmittance;
    return output;
}


//--------------------- for Fullscreen sampling ---------------------------

//float sampleDensityFog(float3 position, Texture3D noiseTexture, float3 offsetPos);

//FullscreenPixelOutput main(FullscreenVertexToPixel input)
//{
//    FullscreenPixelOutput output;
//    float4 col = FullscreenTexture1.Sample(aDefaultSampler, input.UV).rgba;
    
//    float4 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, input.UV).xyzw;
//    float3 rayOrigin = CameraPosition.xyz;
//    //float3 rayDir = normalize(input.viewVector);
   
//    //float3 worldPos = rayOrigin + CameraForward.xyz * FarPlane;
//    //float3 rayDir = normalize(worldPos.xyz - rayOrigin);
    
//    float2 uv = (input.UV * 2.0 - resolution.zw) / resolution.w;
//    float3 rayDir = normalize(float3(uv, 1));
    
//    //float camFOVWidth = 3.14 / 3.5;
//    //float camWidthScale = 2.0 * tan(camFOVWidth / 2.0);
//    //float camHeightScale = camWidthScale * resolution.w / resolution.z;
    
//    ////float2 xy = 2.0 * (input.Position.xy / resolution.zw) - 1.0;
//    //float2 xy = /*(input.Position.xy / resolution.zw)*/input.UV * 2.0 - 1.0;
//    //float3 rayDir = normalize(CameraForward + CameraRight * xy.x * camWidthScale + CameraUp * xy.y * camHeightScale);
//    // Compute depth
//    //float depth = GetLinDepth(worldPosition) * length(input.viewVector);
    
//    // Calculate ray-box intersection
//    float2 rayBoxInfo = rayBoxDist(BoundsMin.xyz, BoundsMax.xyz, rayOrigin, rayDir);
//    float dstToBox = rayBoxInfo.x;
//    float dstInsideBox = rayBoxInfo.y;
    
//     // Ensure ray marching starts from a fixed height
//    //float startHeight = max(MinViewHeight, dstToBox);
//    // Entry point into the cloud volume
//    float3 entryPoint = rayOrigin + rayDir * dstToBox /** MinViewHeight*/;
    
//    // Perform ray marching
//    float dstTravelled = 0;
//    float stepSize = dstInsideBox / numSteps;
//    //float dstLimit = min(depth - dstToBox, dstInsideBox);
    
//    float3 animatedFogPos = float3(CloudOffset.x, CloudOffset.y + time, CloudOffset.z) * 0.01;
//    // Phase function makes clouds brighter around sun
//    float cosAngle = dot(rayDir, dirLightPosition.xyz);
//    float phaseVal = phase(cosAngle);
    
//    float transmittance = 1;
//    float3 lightEnergy = 0;
    
//    [loop]
//    while (dstTravelled < dstInsideBox)
//    {
//        float3 rayPos = entryPoint + rayDir * dstTravelled;
//        float density = sampleDensity(rayPos, perlinNoise, animatedFogPos);
                    
//        if (density > 0)
//        {

//            float lightTransmittance = lightmarch(rayPos, cosAngle, perlinNoise, animatedFogPos);
//            lightEnergy += density * stepSize * transmittance * lightTransmittance * phaseVal;
//            transmittance *= exp(-density * stepSize * lightAbsorptionThroughCloud);
                    
//                        // Early exit
//            if (transmittance < 0.01)
//            {
//                break;
//            }
//        }
                    
//        dstTravelled += stepSize;
//    }
    
//    //float dstFog = 1 - exp(-max(0, depth) * 8 * .0001);
//    //col = col * (1 - dstFog);
//    float4 fogCol = float4(lightEnergy * float3(0.91, 0.898, 0.98), 0) /** (1 - dstFog)*/;
//    output.color = col * transmittance + fogCol;
//    output.color.a = 0;
//    return output;
//}



//float sampleDensityFog(float3 position, Texture3D noiseTexture, float3 offsetPos)
//{
//    //float3 size = BoundsMax.xyz - BoundsMin.xyz;
//    //float3 uvw = (size * 0.5f + position) * CloudScale * 0.001;
//    //float3 shapeSamplePos = uvw + offsetPos;
//    //float density = noiseTexture.Sample(aDefaultSampler, shapeSamplePos).r;
//    //density = (density - DensityThreshold) * DensityMultiplier;
//    //return density;
    
//    float3 wTexturesize;
//    noiseTexture.GetDimensions(wTexturesize.x, wTexturesize.y, wTexturesize.z);
//    //for x
//    float boxXlength = (BoundsMax.x + abs(BoundsMin.x));
//    float worldPosInBoxSpaceX = position.x + abs(BoundsMin.x);
//    float normalizedXposition = worldPosInBoxSpaceX / boxXlength;
//    float pixelCoordX = normalizedXposition * wTexturesize.x;
//    //for y
//    float boxYlength = (BoundsMax.y + abs(BoundsMin.y));
//    float worldPosInBoxSpaceY = position.y + abs(BoundsMin.y);
//    float normalizedYposition = worldPosInBoxSpaceY / boxYlength;
//    float pixelCoordY = normalizedYposition * wTexturesize.y;
//    //for z
//    float boxZlength = (BoundsMax.z + abs(BoundsMin.z));
//    float worldPosInBoxSpaceZ = position.z + abs(BoundsMin.z);
//    float normalizedZposition = worldPosInBoxSpaceZ / boxZlength;
//    float pixelCoordZ = normalizedZposition * wTexturesize.z;
    
//    float3 newPos = (pixelCoordX, pixelCoordY, pixelCoordZ);
    
//    float3 uvw = position * CloudScale * 0.001 + offsetPos;
//    float shape = noiseTexture.SampleLevel(aDefaultSampler, uvw, 0).r; //should add shapenoise??
//    float density = (shape - DensityThreshold) * DensityMultiplier;
//    return density;
//}