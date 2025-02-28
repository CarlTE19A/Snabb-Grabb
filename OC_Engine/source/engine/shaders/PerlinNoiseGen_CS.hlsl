#include "common.hlsli"

float perlinNoisef(float3 position, int frequency, int octaveCount, float persistence, float lacunarity, uint seed);
float perlinNoisef(float3 position, uint seed);
[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 iResolution;
    cOutPutPerlin.GetDimensions(iResolution.x, iResolution.y, iResolution.z);
    float3 position = DTid / iResolution;
    //position.x *= iResolution.x / iResolution.y;
    uint seed = 0x578437adU; // can be set to something else if you want a different set of random values you
    //float frequency = 16.0; stupid
    //float value = perlinNoisef(position * frequency, seed); // single octave perlin noise
    //float value = perlinNoise(vec3(position, iTime * 0.25), 1, 6, 0.5, 2.0, seed); // multiple octaves
    float value = perlinNoisef(position, 1, 6, 0.5, 2.0, seed); // multiple octaves
    value = (value + 1.0) * 0.5; // convert from range [-1, 1] to range [0, 1]
    cOutPutPerlin[DTid] = float4(value, value, value, 1.0);
}

uint hash(uint x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    // process input
    uint k = x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // some final mixing
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

uint hash(uint3 x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    // process first vector element
    uint k = x.x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // process second vector element
    k = x.y;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // process third vector element
    k = x.z;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
	// some final mixing
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}


float3 gradientDirection(uint hash)
{
    switch (int(hash) & 15)
    { // look at the last four bits to pick a gradient direction
        case 0:
            return float3(1, 1, 0);
        case 1:
            return float3(-1, 1, 0);
        case 2:
            return float3(1, -1, 0);
        case 3:
            return float3(-1, -1, 0);
        case 4:
            return float3(1, 0, 1);
        case 5:
            return float3(-1, 0, 1);
        case 6:
            return float3(1, 0, -1);
        case 7:
            return float3(-1, 0, -1);
        case 8:
            return float3(0, 1, 1);
        case 9:
            return float3(0, -1, 1);
        case 10:
            return float3(0, 1, -1);
        case 11:
            return float3(0, -1, -1);
        case 12:
            return float3(1, 1, 0);
        case 13:
            return float3(-1, 1, 0);
        case 14:
            return float3(0, -1, 1);
        case 15:
            return float3(0, -1, -1);
    }
}

float interpolate(float value1, float value2, float value3, float value4, float value5, float value6, float value7, float value8, float3 t)
{
    return lerp(
        lerp(lerp(value1, value2, t.x), lerp(value3, value4, t.x), t.y),
        lerp(lerp(value5, value6, t.x), lerp(value7, value8, t.x), t.y),
        t.z
    );
}

float3 fade(float3 t)
{
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float perlinNoisef(float3 position, uint seed)
{
    float3 floorPosition = floor(position);
    float3 fractPosition = position - floorPosition;
    uint3 cellCoordinates = uint3(floorPosition);
    float value1 = dot(gradientDirection(hash(cellCoordinates, seed)), fractPosition);
    float value2 = dot(gradientDirection(hash((cellCoordinates + uint3(1, 0, 0)), seed)), fractPosition - float3(1, 0, 0));
    float value3 = dot(gradientDirection(hash((cellCoordinates + uint3(0, 1, 0)), seed)), fractPosition - float3(0, 1, 0));
    float value4 = dot(gradientDirection(hash((cellCoordinates + uint3(1, 1, 0)), seed)), fractPosition - float3(1, 1, 0));
    float value5 = dot(gradientDirection(hash((cellCoordinates + uint3(0, 0, 1)), seed)), fractPosition - float3(0, 0, 1));
    float value6 = dot(gradientDirection(hash((cellCoordinates + uint3(1, 0, 1)), seed)), fractPosition - float3(1, 0, 1));
    float value7 = dot(gradientDirection(hash((cellCoordinates + uint3(0, 1, 1)), seed)), fractPosition - float3(0, 1, 1));
    float value8 = dot(gradientDirection(hash((cellCoordinates + uint3(1, 1, 1)), seed)), fractPosition - float3(1, 1, 1));
    return interpolate(value1, value2, value3, value4, value5, value6, value7, value8, fade(fractPosition));
}

float perlinNoisef(float3 position, int frequency, int octaveCount, float persistence, float lacunarity, uint seed)
{
    float value = 0.0;
    float amplitude = 1.0;
    float currentFrequency = float(frequency);
    uint currentSeed = seed;
    for (int i = 0; i < octaveCount; i++)
    {
        currentSeed = hash(currentSeed, 0x0U); // create a new seed for each octave
        value += perlinNoisef(position * currentFrequency, currentSeed) * amplitude;
        amplitude *= persistence;
        currentFrequency *= lacunarity;
    }
    return value;
}

