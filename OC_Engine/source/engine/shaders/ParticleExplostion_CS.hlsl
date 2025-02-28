#include "ParticleCommon.hlsli"
#include "common.hlsli"
#include "DeferredCommon.hlsli"

groupshared uint rand_state = 12;

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
    float tempFloat = float(rand_state * (1 / 4294967296.0f));
    return tempFloat;
}

float3 generate_random_point_in_cone(float4x4 cone_matrix, float height, float angle)
{
    float r1 = rand_float();
    float r2 = rand_float();
    float phi = rand_float() * 2 * M_PI;

    float distance = r1 * height;
    float radius = /*sin(*/radians(angle) /*)*/ * distance;

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
    
    //float4 endColor = float4(0, 0, 0, 1);
    
    particle.size = lerp(size, 0, particle.age / particle.maxLifetime);
        
    //particle.color = endColor;
    
    if (particle.age >= particle.maxLifetime)
    {
        
        rand_state = wang_hash((GTid.x * DTid.x + particle.age + particle.position.x + particle.position.y + particle.position.z) * rand_state);
        
        //particle.color = float4(0, 0, 0, 0);
        //rw_particle_buffer[DTid.x] = particle;
        float tempAngel = maxAngel * rand_float();
        float tempPositionX = (500 * rand_float()) * cos(tempAngel);
        float tempPositionY = (500 * rand_float()) * sin(tempAngel);
        uint particlesCreated = 0;
        InterlockedAdd(RW_totalParticlesCreated[0], 1, particlesCreated);
            //GroupMemoryBarrierWithGroupSync();
        if (particlesCreated < 2500000)
        {
            float4 transposedPosition;
            transposedPosition.x = centerPos[0].w;
            transposedPosition.y = centerPos[1].w;
            transposedPosition.z = centerPos[2].w;
            transposedPosition.w = centerPos[3].w;
                
            particle.position = transposedPosition;
    
            float3 cone_dir = generate_random_point_in_cone(centerPos, 1.0f, angelOfParticleTrail);
            float3 dir = normalize( /*particle.position.xyz - */cone_dir);
        
            float offsetMagnitude = 0.1;
            float3 offset = generate_offset_vector(offsetMagnitude, normalize(dir));

            particle.position.xyz += offset;
                
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
            
            particle.velocity = float4(dir, 1.0f) * -50;
            particle.maxLifetime = maxLifeTime /* * rand_float()*/;
            particle.color = particlecolor;
            particle.age = 0;
        }
        rw_particle_buffer[DTid.x] = particle;
    }
    else
    {
    //    else if (particle.shouldBeStatic == 0)
    //    {  
        float4 vertexViewPos = mul(worldToCameraSpace, particle.position);
        float4 vertexProjectionPosTmp = mul(cameraToProjection, vertexViewPos);
        float3 vertexProjectionPos = vertexProjectionPosTmp.xyz / vertexProjectionPosTmp.w;
        
        float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * vertexProjectionPos.xy;
        float3 worldPos = gBufferPositionTexture.SampleLevel(aDefaultSampler, sampleCoord, 0).xyz;
        //float3 worldNormal = normalize(2.f * gBufferNormalTexture.SampleLevel(aDefaultSampler, sampleCoord).xyz - 1.f, 0);
        float3 worldNormal = normalize(2.f * gBufferNormalTexture.SampleLevel(aDefaultSampler, sampleCoord, 0).xyz - 1.f);
        //float3 normal = normalize(2.f * gBufferNormalTexture.Sample(aDefaultSampler, uv.xy).xyz - 1.f);
        float velDOT;
        
    //    float4 pos = particle.position;
    //    float4 vel = particle.velocity;
    
    // Calculate the vector from the particle to the explosion center
        float3 distanceToExplosion = particle.position.xyz - explostionPosition.xyz;

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

    ////// Apply gravity
    ////    particle.velocity.y -= gravityForce * g_paramf.x;
                    
        if (clamp(vertexProjectionPos.x, -1.0, 1.0) == vertexProjectionPos.x && clamp(vertexProjectionPos.y, -1.0, 1.0) == vertexProjectionPos.y)
        {
            float4 vertexViewPosWorld = mul(worldToCameraSpace, worldPos);
            float4 vertexProjectionPosTmpWorld = mul(cameraToProjection, vertexViewPosWorld);
            float3 vertexProjectionPosWorld = vertexProjectionPosTmpWorld.xyz / vertexProjectionPosTmpWorld.w;
        
            float4 particlePositionVS = mul(worldToCameraSpace, float4(particle_buffer[DTid.x].position.xyz, 1.0));
            float4 particlePositionCS = mul(cameraToProjection, particlePositionVS);
            float2 screenSpacePosition = particlePositionCS.xy / particlePositionCS.w;
                
            float3 sub = particle.position.xyz - worldPos;
            float distance = (dot(normalize(worldNormal), sub));
                
            if (dot(worldNormal, particle.velocity.xyz) < 0.f && distance <= 0.0f && distance > -10.0f)
            {
                float velLength = length(particle.velocity.xyzw);
                float3 dir = normalize(particle.velocity.xyz);
                float3 normal = worldNormal;
                
                
                //particle.velocity =  particle.velocity * -0.45f; /*float4(0, 0, 0, 0);*/ /*float4(0, 0, 0, 0);*/ /*0.01f **/ /* + 0.99f * particle.position.xyz*/ /*;*/
                particle.velocity.xyz = velLength * normalize(dir - 2.f * dot(dir, normal) * normal) * 0.2f;
               // particle.velocity.xyz = particle.velocity.xyz * -cross(normalize(particle.velocity.xyz), normalize(worldNormal));
                //particle.size += 5;22
                velDOT = dot(worldNormal, particle.velocity.xyz);
                if (velDOT > -0.1f && velDOT < 0.1f)
                {
                    particle.velocity = float4(0, 0, 0, 0);
                    particle.shouldBeStatic = 1;
                }
            }
        }
        
    
                   
    //        particle.position.xyz += particle.velocity.xyz * g_paramf.x;
        
        
    //    /*
    //    float4 vertexViewPos = mul(worldToCameraSpace, particle.position);
    //    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
    //    vertexProjectionPos /= vertexProjectionPos.w;
        
    //    float3 worldPos = gBufferPositionTexture.SampleLevel(aDefaultSampler, vertexProjectionPos.xy / resolution.zw, 0).xyz;
        
    //    particle.position.xyz = 0.01f * worldPos + 0.99f * particle.position.xyz;
    //    */
        
    //    }
        
        /*
        float testDepth = GetLogDepth(particle.position);
        //gBufferPositionTexture.SampleLevel();
        float testDpeth2 = GetLogDepth(gBufferPositionTexture.SampleLevel(aDefaultSampler, vertexProjectionPos.xy / resolution.zw, 0).xyzw);
                       
        if (testDepth <= testDpeth2)
        {
            particle.velocity = float4(0, 0, 0, 0);

        }
        */

    //particle.color.a *= 1.0 - (particle.time / particle.maxLifetime);
            //InterlockedAdd(particlefrequency, g_paramf.x);
    //if ( /*g_accumulatedTime <= */particlefrequency <= 0.0f /*<= 0.f*/ /*particle.maxLifetime*/)
    //{
    //    //Createparticle(Gid, DTid, GTid, GI);
    //}
    //}            
    
        
        
        float dragCoefficient = 0.95f /* Your chosen drag coefficient, e.g., 0.1 */;
    
    // Apply gravity
        particle.velocity.y -= gravityForce * g_paramf.x;

    // Calculate the drag force based on the current velocity
    // For a simple linear drag model: F_drag = -c * v
    // Ensure to calculate drag for each component of the velocity vector
        float3 dragForce;
        //dragForce.x = -dragCoefficient * particle.velocity.x;
        //dragForce.y = -dragCoefficient * particle.velocity.y;
        //dragForce.z = -dragCoefficient * particle.velocity.z;
        
        dragForce.x = -dragCoefficient * particle.velocity.x;
        dragForce.y = -dragCoefficient * particle.velocity.y;
        dragForce.z = -dragCoefficient * particle.velocity.z;

    // Apply the drag force to the particle's velocity
        particle.velocity.x += dragForce.x * g_paramf.x; // Apply drag to x velocity
        particle.velocity.y += dragForce.y * g_paramf.x; // Apply drag to y velocity, note: This is in addition to gravity
        particle.velocity.z += dragForce.z * g_paramf.x; // Apply drag to z velocity
                     
        particle.velocity.y -= gravityForce * g_paramf.x;
        
        particle.position += particle.velocity * g_paramf.x;
                
        rw_particle_buffer[DTid.x] = particle;
    }
    GroupMemoryBarrierWithGroupSync();
    
}


