#include "ParticleCommon.hlsli"

void Createparticle(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    Particle newParticle = particle_buffer[DTid.x];

    if (newParticle.age >= newParticle.maxLifetime)
    {
        uint shit = 0;
        InterlockedAdd(RW_totalParticlesCreated[0], 1, shit);
                
        if (shit <= numberOfParticlesToCreatePerCreateCall)
        {
            float radius = 1;

            float x = radius * sin(DTid.x % 360);
            float y = 0;
            float z = radius * cos(DTid.x % 360);

            float3 vDelta = float3(-x, y, -z);

            float3 transposedPosition = 0;
            transposedPosition.x = centerPos[0].w;
            transposedPosition.y = centerPos[1].w;
            transposedPosition.z = centerPos[2].w;
                
            float3 vPos = transposedPosition + vDelta;
                
            float3 vRadialDir = normalize(transposedPosition - vPos);
            float3 vFinalVelocity = 150.f * vRadialDir;

            newParticle.position = float4(vPos, 1.0f);
            newParticle.color = particlecolor;
            newParticle.size = size;
            newParticle.age = 0.0f;
            newParticle.maxLifetime = maxLifeTime;

            newParticle.velocity = float4(vFinalVelocity, 0.0f);
                    
            rw_particle_buffer[DTid.x] = newParticle;
        }
    }
}
    
[numthreads(blocksize, 1, 1)]
 void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    Particle particle = particle_buffer[DTid.x];
    
    if (particle.age >= particle.maxLifetime)
    {
        particle.color = float4(0, 0, 0, 0);
        rw_particle_buffer[DTid.x] = particle;
    }
    else
    {
    // Apply gravity
        particle.velocity.y -= gravityForce * g_paramf.x;

    // Update position based on velocity
        particle.position.xyz += particle.velocity.xyz * g_paramf.x;

        particle.age += g_paramf.x;
            
        rw_particle_buffer[DTid.x] = particle;
    }
        //InterlockedAdd(particlefrequency, g_paramf.x);
    if ( /*g_accumulatedTime <= */particlefrequency <= 0.0f /*<= 0.f*/ /*particle.maxLifetime*/)
    {
        Createparticle(Gid, DTid, GTid, GI);
    }
    GroupMemoryBarrierWithGroupSync();
}


