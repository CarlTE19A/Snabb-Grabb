#include "common.hlsli"

float worley(float3 p, float scale);
float worleyFbm(float3 p, float freq);
float gradientNoise(float3 x, float freq);
float perlinfbm(float3 p, float freq, int octaves);
float remap(float x, float a, float b, float c, float d);
float3 hash33(float3 p3);

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 res;
    cOutPut.GetDimensions(res.x, res.y, res.z);
    float3 uv = float3(DTid) / res;
    
    float slices = 256.0;
    float freq = 10.0;
    
    float pfbm = lerp(1.0, perlinfbm(uv, 4.0, 7), 0.5);
    pfbm = abs(pfbm * 2.0 - 1.0); // billowy perlin noise
    float4 color = float4(0,0,0,0);
    color.g += worleyFbm(uv.xyz, freq);
    color.b += worleyFbm(uv.xyz, freq * 2.0);
    color.a += worleyFbm(uv.xyz, freq * 4.0);
    color.r += remap(pfbm, 0.0, 1.0, color.g, 1.0);
    cOutPut[DTid.xyz] = color;
}

float worley(float3 p, float scale)
{
    float3 id = floor(p);
    float3 fd = frac(p);
    float minimalDist = 10000.0;
    
    for (float x = -1.; x <= 1.; x++)
    {
        for (float y = -1.; y <= 1.; y++)
        {
            for (float z = -1.; z <= 1.; z++)
            {

                float3 coord = float3(x, y, z);
                float3 rId = hash33(fmod(id + coord, float3(scale, scale, scale))) * 0.5 + 0.5;
                rId += coord;
                float3 d = fd - rId;
                minimalDist = min(minimalDist, dot(d, d));

            } //z
        } //y
    } //x
    
    return 1.0 - minimalDist;
}

float worleyFbm(float3 p, float freq)
{
    return worley(p * freq, freq) * 0.625 +
        	 worley(p * freq * 2.0, freq * 2.0) * 0.25 +
        	 worley(p * freq * 4.0, freq * 4.0) * 0.125;
}

float perlinfbm(float3 p, float freq, int octaves)
{
    float G = exp2(-0.85);
    float amp = 1.0;
    float noise = 0.0;
    for (int i = 0; i < octaves; ++i)
    {
        noise += amp * gradientNoise(p * freq, freq);
        freq *= 2.0;
        amp *= G;
    }
    
    return noise;
}

float gradientNoise(float3 x, float freq)
{
    // grid
    float3 p = floor(x);
    float3 w = frac(x);
    // quintic interpolant
    float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
    // gradients
    float3 ga = hash33(fmod(p + float3(0.0, 0.0, 0.0), freq));
    float3 gb = hash33(fmod(p + float3(1.0, 0.0, 0.0), freq));
    float3 gc = hash33(fmod(p + float3(0.0, 1.0, 0.0), freq));
    float3 gd = hash33(fmod(p + float3(1.0, 1.0, 0.0), freq));
    float3 ge = hash33(fmod(p + float3(0.0, 0.0, 1.0), freq));
    float3 gf = hash33(fmod(p + float3(1.0, 0.0, 1.0), freq));
    float3 gg = hash33(fmod(p + float3(0.0, 1.0, 1.0), freq));
    float3 gh = hash33(fmod(p + float3(1.0, 1.0, 1.0), freq));
    // projections
    float va = dot(ga, w - float3(0.0, 0.0, 0.0));
    float vb = dot(gb, w - float3(1.0, 0.0, 0.0));
    float vc = dot(gc, w - float3(0.0, 1.0, 0.0));
    float vd = dot(gd, w - float3(1.0, 1.0, 0.0));
    float ve = dot(ge, w - float3(0.0, 0.0, 1.0));
    float vf = dot(gf, w - float3(1.0, 0.0, 1.0));
    float vg = dot(gg, w - float3(0.0, 1.0, 1.0));
    float vh = dot(gh, w - float3(1.0, 1.0, 1.0));
    // interpolation
    return va +
           u.x * (vb - va) +
           u.y * (vc - va) +
           u.z * (ve - va) +
           u.x * u.y * (va - vb - vc + vd) +
           u.y * u.z * (va - vc - ve + vg) +
           u.z * u.x * (va - vb - ve + vf) +
           u.x * u.y * u.z * (-va + vb + vc - vd + ve - vf - vg + vh);
}

float remap(float x, float a, float b, float c, float d)
{
    return (((x - a) / (b - a)) * (d - c)) + c;
}

float3 hash33(float3 p)
{
    uint3 q = uint3(int3(p)) * uint3(1103515245u, 12345u, 314159265u);
    q = (q.x ^ q.y ^ q.z) * uint3(1103515245u, 12345u, 314159265u);
    return -1.0 + 2.0 * float3(q) / 4294967296.0;
}