#include "ParticleCommon.hlsli"


[numthreads(blocksize, 1, 1)]
 void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    //uint index = GTid.x * blocksize + DTid.x;
    Particle particle = particle_buffer[DTid.x];
            
    //else
    //{
        //if (particle.time < particle.maxLifetime)
        //{
    // Each thread of the CS updates one of the particles
    float4 pos = particle.position;
    float4 vel = particle.velocity;
    
        //particle.time += g_paramf.x;                
    float3 accel = 0;
    float mass = g_fParticleMass;
    uint numTiles = (g_param.x + blocksize) / blocksize; // Calculate the number of tiles needed
  
    // Calculate the vector from the particle to the explosion center
    float3 distanceToExplosion = particle.position.xyz - centerPos[3].xyz;

    // Calculate the squared distance (magnitude) from the explosion center
    float distanceSquared = dot(distanceToExplosion, distanceToExplosion);

    // Calculate the normalized direction vector towards the explosion center
    float3 explosionDirection = normalize(distanceToExplosion);

    float falloffFactor = 1.0f / max(1.0f, distanceSquared / (falloffDistance * falloffDistance));
    
    // Calculate the explosive force based on the direction and distance
    //float3 explosionForce = explosionDirection * explosionStrength / max(1.0f, distanceSquared / (falloffDistance * falloffDistance));
    float3 explosionForce = explosionDirection * explosionStrength * falloffFactor;

    // Apply explosive force to velocity
    particle.velocity.xyz += /*explosionDirection * (particle.velocity.xyz +*/explosionForce * g_paramf.x;

    // Apply gravity
    particle.velocity.y -= gravityForce * g_paramf.x;

    // Update position based on velocity
    particle.position.xyz += particle.velocity.xyz * g_paramf.x;

    particle.age += g_paramf.x;
    
    rw_particle_buffer[DTid.x] = particle;
    
    GroupMemoryBarrierWithGroupSync();
}


