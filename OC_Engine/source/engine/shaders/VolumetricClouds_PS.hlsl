#include "VolumetricsCommon.hlsli"


FullscreenPixelOutput main(VertexToPixelCube input)
{
    FullscreenPixelOutput output;
    float4 worldPosition = input.WorldPosition;
    float3 rayOrigin = CameraPosition.xyz;
    float3 rayDir = normalize(worldPosition.xyz - rayOrigin);
  
    // Calculate ray-box intersection
    float2 rayBoxInfo = rayBoxDist(BoundsMin.xyz, BoundsMax.xyz, rayOrigin, rayDir);
    float dstToBox = rayBoxInfo.x;
    float dstInsideBox = rayBoxInfo.y;
    
    // Entry point into the cloud volume
    float3 entryPoint = rayOrigin + rayDir * dstToBox;
    
    // Phase function makes clouds brighter around sun
    float3 dirToLight = normalize(dirLightPosition.xyz - entryPoint);
    float cosAngle = dot(rayDir, dirToLight);
    float phaseVal = PhaseFunction(cosAngle, phaseParams.x, phaseParams.y, phaseParams.z, phaseParams.w);
    
    float dstTravelled = rand_1_05(input.UV * 3.0f);
    float stepSize = dstInsideBox / numSteps;
    float transmittance = 1;
    float3 lightEnergy = 0;
    float3 animatedCloudPos = float3(CloudOffset.x + time, CloudOffset.y, CloudOffset.z + time) * 0.01;
    [loop]
    while (dstTravelled < dstInsideBox)
    {
        float3 rayPos = entryPoint + rayDir * dstTravelled;
        float density = sampleDensity(rayPos, worleyNoise, animatedCloudPos);
        density = TrimCloud(rayPos, density, density);
                    
        if (density > 0)
        {
            float lightTransmittance = lightmarch(rayPos, cosAngle, worleyNoise, animatedCloudPos);
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
    
    output.color.rgb = lightEnergy * dLightColorAndIntensity.xyz;
    output.color.a = 1 - transmittance;
    return output;
}

 //float cloudDepth = GetLogDepth(input.WorldPosition);
    //float worldDepth = GetLogDepth(gBufferPositionTexture.Sample(aDefaultSampler, input.UV).xyzw);
    //float dstFog = 1 - exp(-max(0, worldDepth) * 8 * .0001);
    
    //float spacing = worldDepth - cloudDepth;
    //float fade = saturate(spacing / dstFog);
    //if (cloudDepth > worldDepth)
    //{
    //    alpha = lerp(alpha, dstFog, 0.2f);
    //}
    
    //float3 backgroundWorldPos = gBufferPositionTexture.Sample(aDefaultSampler, uv).xyz;
    //float backgrounDistance = length(backgroundWorldPos - input.WorldPosition.xyz);
    //float scaledBackroundDistance = backgrounDistance / 300.f;
    //scaledBackroundDistance = max(0.f, scaledBackroundDistance);
    //float closeToBackgroundFade = scaledBackroundDistance / (1.f + scaledBackroundDistance);