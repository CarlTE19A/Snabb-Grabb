#include "common.hlsli"
#include "DeferredCommon.hlsli"

float lightmarch(float3 position, float costH);
float2 rayBoxDist(float3 boundsMin, float3 boundsMax, float3 rayOrigin, float3 rayDir);
float sampleDensity(float3 position, Texture3D noiseTexture, float3 offsetPos);
float4x4 inverse(float4x4 m);
float phase(float a);
float hg(float a, float g);
float TrimCloud(float3 aPos, float aDensity, float aDetailFBM);
float EdgeFallof(float3 aPos);
float HeightFallof(float3 aPos);
float MultipleOctaveScattering(float aDensity, float mu);
float beer(float d);
float rand_1_05(in float2 uv);
float PhaseFunction(float costh, float aFrontScatter, float aBackScatter, float aBaseBright, float aPhaseFac);


float remap(float v, float minOld, float maxOld, float minNew, float maxNew)
{
    return minNew + (v - minOld) * (maxNew - minNew) / (maxOld - minOld);
}


float2 rayBoxDist(float3 boundsMin, float3 boundsMax, float3 rayOrigin, float3 rayDir)
{
    float3 t0 = (boundsMin - rayOrigin) / rayDir;
    float3 t1 = (boundsMax - rayOrigin) / rayDir;
    float3 tmin = min(t0, t1);
    float3 tmax = max(t0, t1);
    
    float dstA = max(max(tmin.x, tmin.y), tmin.z);
    float dstB = min(tmax.x, min(tmax.y, tmax.z));
    
    float dstToBox = max(0, dstA);
    float dstInsideBox = max(0, dstB - dstToBox);
    return float2(dstToBox, dstInsideBox);
}

float beer(float d)
{
    float beer = exp(-d);
    return beer;
}

float Powder(float aDist)
{
    return 1.0f - exp(-aDist * 2.0f);
}

float sampleDensity(float3 position, Texture3D noiseTexture, float3 offsetPos)
{
    float3 size = BoundsMax.xyz - BoundsMin.xyz;
    float3 uvw = (size * 0.5f + position) * CloudScale * 0.001;
    float3 shapeSamplePos = uvw + offsetPos;
    float density = noiseTexture.Sample(aDefaultSampler, shapeSamplePos).r;
    density = (density - DensityThreshold) * DensityMultiplier;
    return density;
}
// Calculate proportion of light that reaches the given point from the lightsource
float lightmarch(float3 p, float costH, Texture3D aNoiseTexture, float3 offsetPos)
{
    float3 dirToLight = normalize(p - dirLightPosition.xyz);
    float dstInsideBox = rayBoxDist(BoundsMin.xyz, BoundsMax.xyz, p, dirToLight).y;
                
    float stepSize = dstInsideBox / numStepsLight;
    float totalDensity = 0.0;
    float dstTravelled = 0.0;
    [loop]
    for (int step = 0; step < numStepsLight; step++)
    {
        if (dstTravelled > dstInsideBox)
            break;
        dstTravelled += dirToLight * stepSize;
        p += dstTravelled;
        float density = sampleDensity(p, aNoiseTexture, offsetPos);
        totalDensity += max(0, density * stepSize);
    }
    
    float densMul = totalDensity * lightAbsorptionThroughCloud;
    float beersLaw = MultipleOctaveScattering(densMul, costH);
    float transmittance = beersLaw * lerp(2.0f * Powder(densMul), float3(1.0f, 1.0f, 1.0f),
                                     remap(costH, -1.0f, 1.0f, 0.0f, 1.0f));
    return darknessThreshold + transmittance * (1 - darknessThreshold);
}

float phase(float a)
{
    float blend = 0.5;
    float hgBlend = hg(a, phaseParams.x) * (1 - blend) + hg(a, -phaseParams.y) * blend;
    return phaseParams.z + hgBlend * phaseParams.w;
}

 // Henyey-Greenstein
float hg(float a, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * 3.1415 * pow(1.0 + g2 - 2.0 * g * (a), 1.5));
}

float TrimCloud(float3 aPos, float aDensity, float aDetailFBM)
{
    float oneMinuseShape = 1.0 - aDensity;
    float detailErodeWeights = oneMinuseShape * oneMinuseShape * oneMinuseShape;
    float cloudDensity = aDensity - (1.0f - aDetailFBM) * detailErodeWeights * darknessThreshold;
    
    return cloudDensity * ((EdgeFallof(aPos) * HeightFallof(aPos)));
}

float EdgeFallof(float3 aPos)
{
    const float containerEdgeFadeDst = 50.0f;
    float dstFromEdgeX = min(containerEdgeFadeDst, min(aPos.x - BoundsMin.x, BoundsMax.x - aPos.x));
    float dstFromEdgeZ = min(containerEdgeFadeDst, min(aPos.z - BoundsMin.z, BoundsMax.z - aPos.z));
    float edgeWeight = min(dstFromEdgeX, dstFromEdgeX) / containerEdgeFadeDst;
    return edgeWeight;
}

float HeightFallof(float3 aPos)
{
    float3 size = BoundsMax.xyz - BoundsMin.xyz;
    
    float minHeight = 0.2f;
    float maxHeight = 0.7f;
    float percent = (aPos.y - BoundsMin.y) / size.y;
    
    float percentMin = saturate(remap(percent, 0.0f, minHeight, 0.0f, 1.0f));
    float percentMax = saturate(remap(percent, 1.0f, maxHeight, 0.0f, 1.0f));
    float gradient = percentMin * percentMax;
    return gradient;
}

const float DUAL_LOBE_WEIGHT = 0.7;
float DualLob_HG(float g, float costh)
{
    return lerp(hg(g, costh), hg(g, costh), DUAL_LOBE_WEIGHT);
}

float PhaseFunction(float g, float costh)
{
    return DualLob_HG(g, costh);
}

float PhaseFunction(float costh, float aFrontScatter, float aBackScatter, float aBaseBright, float aPhaseFac)
{
    float hgBlend = hg(aFrontScatter, costh) * (1 - DUAL_LOBE_WEIGHT) + hg(-aBackScatter, costh) * DUAL_LOBE_WEIGHT;
    return aBaseBright + hgBlend * aPhaseFac;
}

#define EXTINCTION_MULT 0.0f
float MultipleOctaveScattering(float aDensity, float mu)
{
    const float atten = 0.2f;
    const float contribution = 0.4f;
    const float phaseAtten = 0.1f;
    
    const float scatteringOctaves = 4.0f;
    float a = 1.0f;
    float b = 1.0f;
    float c = 1.0f;
    
    float lum = 0.0f;
    for (float i = 0.0f; i < scatteringOctaves; i++)
    {
        float phaseFunc = PhaseFunction(0.3f * c, mu);
        float beers = beer(aDensity * EXTINCTION_MULT * a);
        
        lum += b * phaseFunc * beers;
        
        a *= atten;
        b *= contribution;
        c *= (1.0f - phaseAtten);
    }

    return lum;
}

float rand_1_05(in float2 uv)
{
    float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}