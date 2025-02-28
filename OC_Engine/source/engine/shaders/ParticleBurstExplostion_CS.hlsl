#include "ParticleCommon.hlsli"

[numthreads(blocksize, 1, 1)]
 void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    //uint index = GTid.x * blocksize + DTid.x;
    Particle particle = particle_buffer[DTid.x];
     
    // Calculate the vector from the particle to the explosion center
    float3 distanceToExplosion = particle.position.xyz - explostionPosition.xyz;
    
    // Calculate the normalized direction vector towards the explosion center
    float3 explosionDirection = normalize(distanceToExplosion);

    // Apply explosive force to velocity
    particle.velocity.xyz += particle.velocity.xyz * g_paramf.x;

    // Apply gravity
    particle.velocity.y -= gravityForce * g_paramf.x;

    // Update position based on velocity
    particle.position.xyz += explosionDirection * particle.velocity.xyz * g_paramf.x;
           
    particle.age += g_paramf.x;
            
    rw_particle_buffer[DTid.x] = particle;
    
    GroupMemoryBarrierWithGroupSync();
}


