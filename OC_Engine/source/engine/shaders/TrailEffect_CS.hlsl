#include "ParticleCommon.hlsli"

static uint rand_state = 0;

uint rand_lcg(uint state)
{
    // LCG values from Numerical Recipes
    state = 1664525 * state + 1013904223;
    return state;
}

uint rand_xorshift(uint state)
{
    // Xorshift algorithm from George Marsaglia's paper
    state ^= (state << 13);
    state ^= (state >> 17);
    state ^= (state << 5);
    return state;
}

uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float rand_float()
{
    rand_state = rand_xorshift(rand_state);
    float tempFloat = float(rand_state * (1.0f / 4294967296.0f));
    return tempFloat;
}

float3 generate_random_point_in_cone(float4x4 cone_matrix, float height, float angle)
{
    float r2 = rand_float();
    float phi = rand_float() * 2 * M_PI;

    float distance = height;
    float radius = tan(radians(angle)) * height;

    float x = radius * sqrt(r2) * cos(phi);
    float y = radius * sqrt(r2) * sin(phi);
    float z = distance;
    
    //float3 transposedPosition = 0;
    //transposedPosition.x = centerPos[0].w + x;
    //transposedPosition.y = centerPos[1].w + y;
    //transposedPosition.z = centerPos[2].w + z;
    
    // Rotate (x, y, z) to align with the cone's direction vector
    float4 rotatedPos = mul(cone_matrix, float4(x, y, z, 0.0f));

    // Now (x, y, z) is a random point inside the cone
    return float3(rotatedPos.x, rotatedPos.y, rotatedPos.z);
}

float3 generate_offset_vector(float magnitude, float3 mainDirection)
{
    // Generate a random perpendicular vector
    float3 randomDir = normalize(float3(rand_float(), rand_float(), rand_float()) - 0.5f);
    float3 perpDir = normalize(cross(randomDir, mainDirection));
    return perpDir * magnitude;
}

[numthreads(blocksize, 1, 1)]
 void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    Particle particle = particle_buffer[DTid.x];
    
    particle.age += g_paramf.x;
         
// During particle reset
    if (particle.age >= particle.maxLifetime)
    {
        particle.color = float4(0, 0, 0, 0);
        rw_particle_buffer[DTid.x] = particle;
        if (particlefrequency <= 0)
        {
            
            rand_state = wang_hash((GTid.x + (DTid.x + particle.age + GI.x + GTid.x + particle.position.x + particle.position.y + particle.position.z) + Get_Global_Seed()));
        
            float tempAngel = rand_float() * 2 * M_PI;
            float r = sqrt(rand_float());
            float tempPositionX = (maxRadius * r) * cos(tempAngel);
            float tempPositionY = (maxRadius * r) * sin(tempAngel);
            uint particlesCreated = 0;
            InterlockedAdd(RW_totalParticlesCreated[0], 1, particlesCreated);
            //GroupMemoryBarrierWithGroupSync();
            if (particlesCreated < numberOfParticlesToCreatePerCreateCall)
            {
                float4 transposedPosition;
                transposedPosition.x = centerPos[0].w;
                transposedPosition.y = centerPos[1].w;
                transposedPosition.z = centerPos[2].w;
                transposedPosition.w = centerPos[3].w;
                
                particle.position = transposedPosition;
    
                float3 cone_dir = generate_random_point_in_cone(centerPos, 1.0f, angelOfParticleTrail);
                float3 dir = normalize( /*particle.position.xyz - */cone_dir);
        
                //float offsetMagnitude = 0.1;
                //float3 offset = generate_offset_vector(offsetMagnitude, normalize(dir));

                //particle.position.xyz += offset;
                
                
                if (trailEffectType == 1)
                {
                    float3x3 tempRotationMatrix;
                    tempRotationMatrix[0].x = centerPos[0].x;
                    tempRotationMatrix[0].y = centerPos[0].y;
                    tempRotationMatrix[0].z = centerPos[0].z;
                
                    tempRotationMatrix[1].x = centerPos[1].x;
                    tempRotationMatrix[1].y = centerPos[1].y;
                    tempRotationMatrix[1].z = centerPos[1].z;
                
                    tempRotationMatrix[2].x = centerPos[2].x;
                    tempRotationMatrix[2].y = centerPos[2].y;
                    tempRotationMatrix[2].z = centerPos[2].z;
                
                    float3 tempPos2 = float3(tempPositionX, tempPositionY, 0);
                
                    float3 tempPos = float3(0, 0, 0);
                    tempPos = mul(tempRotationMatrix, tempPos2);

                    particle.position.xyz += tempPos;
                }
                
                particle.velocity = float4(dir, 1.0f) * -500;
                particle.maxLifetime = maxLifeTime /* * rand_float()*/;
                particle.color = particlecolor;
                particle.age = 0;
                rw_particle_buffer[DTid.x] = particle;
            }
        }
    } //}
    else
    {
        particle.velocity.y -= gravityForce * g_paramf.x;
        particle.position += particle.velocity * g_paramf.x;
        rw_particle_buffer[DTid.x] = particle;
    }

    GroupMemoryBarrierWithGroupSync();
}

