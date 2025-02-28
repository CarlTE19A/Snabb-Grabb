static float softeningSquared = 0.0012500000 * 0.0012500000;
static float g_fG = 6.67300e-11f * 100.0f;
static float g_fParticleMass = g_fG * 10000.0f * 10000.0f;

#define blocksize 128
groupshared float4 sharedPos[blocksize];

groupshared uint totalParticlesCreated;

#define M_PI 3.14159265358979323846

struct ParticleInput
{
    float4 color : COLOR;
    uint id : SV_VertexID;
};

struct Particle
{
    float4 position;
    float4 velocity;
    float4 color;
    float size;
    float age;
    float maxLifetime;
    int shouldBeStatic;
    //float _rotation;

    //float3 startVelocity;
    //float3 endVelocity;
    //float4 startColor;
    //float4 endColor;
    //float startSize;
    //float endSize;
    //float startRotation;
    //float endRotation;

    //float maxTime;
};

StructuredBuffer<Particle> particle_buffer : register(t15);
RWStructuredBuffer<Particle> rw_particle_buffer : register(u1);

RWStructuredBuffer<uint> RW_totalParticlesCreated : register(u2);

RWStructuredBuffer<uint> RW_global_seed : register(u3);

uint Get_Global_Seed()
{
    uint tempValue;
    InterlockedAdd(RW_global_seed[0], 1, tempValue);
    
    return tempValue;
}

cbuffer cbCS : register(b11)
{
    uint4 g_param; // pcbCS->param[0] = MAX_PARTICLES;
                        // pcbCS->param[1] = dimx;              
    float4 g_paramf; // pcbCS->paramf[0] = 0.1f;
                        // pcbCS->paramf[1] = 1; 
    float explosionStrength;
    
    float falloffDistance;
};

cbuffer ParticleBuffer : register(b12)
{
    float4x4 centerPos;
    float4 explostionPosition;
    float4 particlecolor;
    float gravityForce;
    float size;
    float maxLifeTime;
    float particlefrequency;
    float angelOfParticleTrail;
    float maxRadius;
    float maxAngel;
    int numberOfParticlesToCreatePerCreateCall;
    int maxNumberOfParitcles; //How many particle the the particle system can show at onces
    int trailEffectType;
};

struct ParticlePixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
};

struct ParticlePixelOutput
{
    float4 color : SV_TARGET;
};

struct PosVelo
{
    float4 pos;
    float4 velo;
};

