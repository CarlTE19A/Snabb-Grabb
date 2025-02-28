#include "ParticleCommon.hlsli"
#include "common.hlsli"

ParticlePixelInput main(uint vertex_id : SV_VertexID)
{
    ParticlePixelInput output;

    uint alive_particle_id = vertex_id / 6;
    
    uint particle_id = alive_particle_id;
    uint corner_id = vertex_id % 6;

    Particle particle = particle_buffer[particle_id];
    //particle.size = 10;
    float3 camera_to_particle = particle.position.xyz - CameraPosition.xyz;
    float3 camera_to_particle_normalized = normalize(camera_to_particle);
    float3 camera_right_normalized = normalize(CameraRight.xyz);
    float3 camera_up_normalized = normalize(CameraUp.xyz);
    
    float half_size = particle.size * 0.5f;
    float3 right_offset = camera_right_normalized * half_size;
    float3 up_offset = camera_up_normalized * half_size;
    
    float3 corner_offset = float3(0.0f, 0.0f, 0.0f);
    float2 corner_uv = float2(0.0f, 0.0f);
    if (corner_id == 0 || corner_id == 3)
    {
        // Bot left
        corner_offset = (-right_offset) - up_offset;
        corner_uv = float2(0.0f, 1.0f);
    }
    else if (corner_id == 1)
    {
        // Top left
        corner_offset = (-right_offset) + up_offset;
        corner_uv = float2(0.0f, 0.0f);
    }
    else if (corner_id == 2 || corner_id == 4)
    {
        // Top right
        corner_offset = right_offset + up_offset;
        corner_uv = float2(1.0f, 0.0f);
    }
    else if (corner_id == 5)
    {
        // Bot right
        corner_offset = right_offset - up_offset;
        corner_uv = float2(1.0f, 1.0f);
    }
    
    //float2x2 rotation_matrix = float2x2(
    //    float2(cos(particle._rotation), sin(particle._rotation)),
    //    float2(-sin(particle._rotation), cos(particle._rotation))
    //);
    
    //corner_offset = float3(mul(float2(corner_offset.x, corner_offset.y), rotation_matrix), corner_offset.z);
    
    float4 object_position = float4(particle.position.xyz + corner_offset, 1.0f);
    float4 clip_position = mul(worldToCameraSpace, object_position);
    float4 clip_position1 = mul(cameraToProjection, clip_position);
    
    output.position = clip_position1;
    output.color = particle.color;
    output.uv = corner_uv;
    
    return output;

}