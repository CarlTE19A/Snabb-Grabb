#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"

float3 outline_effect(float _OutlineWidth, float _OutlineSoftness, float _OutlinePower, float3 _OutlineColor, float3 normal, float3 view_dir, out float out_frensel)
{
    float edge1 = 1 - _OutlineWidth;
    float edge2 = edge1 + _OutlineSoftness;
    float fresnel = pow(1.0 - saturate(dot(normal, view_dir)), _OutlinePower);
    out_frensel = fresnel;
    return lerp(1, smoothstep(edge1, edge2, fresnel), step(0, edge1)) * _OutlineColor;
}

#define SAMPLES_COUNT 64 
#define SAMPLES_COUNT_DIV_2 32  
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)  

PixelOutput main(FullscreenVertexToPixel input)
{
    float2 uv = input.Position.xy / resolution.zw;
    float4 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, uv).rgba;
    float4 albedo = gBufferAlbedoTexture.Sample(aDefaultSampler, uv).rgba;
    float3 normal = normalize(2.f * gBufferNormalTexture.Sample(aDefaultSampler, uv).xyz - 1.f);
    float4 material = gBufferMaterialTexture.Sample(aDefaultSampler, uv);
    
    PixelOutput result;
    
    float3 toEye = normalize(CameraPosition.xyz - worldPosition.xyz);
    
    float ambientOcclusion = material.r;
    float1 metalness = material.b;
    float1 roughness = material.g;
    
    float emissive = gBufferEffectsTexture.Sample(aDefaultSampler, uv).r;
    

    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, (float3) metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, (float3) 1 - (float3) metalness);
    
    float3 ambiance = EvaluateAmbiance(
		aEnvironmentTexture, normal, normal,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
    
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, normal, roughness,
		dLightColorAndIntensity.xyz, dLightDir.xyz, toEye.xyz
	);
        
    float shadowFactor = 1.0f;
    float4 directionalLightProjectedPositionTemp = mul(dirLightProjection, worldPosition);
    float3 directionalLightProjectedPosition = directionalLightProjectedPositionTemp.xyz / directionalLightProjectedPositionTemp.w;
  //  if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
		//clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
  //  {
  //      shadowFactor = 0.0f;
  //      float bias = 0.01;
  //      float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy;
        
  //      //float2 offset = float2(i / 5, j / 5) / float(SAMPLES_COUNT_DIV_2) * INV_SAMPLES_COUNT;
  //          //float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy + offset;
  //      sampleCoord.x = dirLightTopLeftXY.x + sampleCoord.x * dirLightViewPortSize.x;
  //      sampleCoord.y = dirLightTopLeftXY.y + sampleCoord.y * dirLightViewPortSize.y;
            
  //      // Convert to texture coordinates
  //      sampleCoord.x /= 16384;
  //      sampleCoord.y /= 16384;

  //      float computedZ = directionalLightProjectedPosition.z;
  //      float shadowMapZ = directionalLightShadowMap.Sample(aDefaultSampler, sampleCoord);
  //      shadowFactor = (computedZ < shadowMapZ + bias) ? 1.0 : 0.0;
  //  }
    
    if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
		clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
    {
        float samples = float(SAMPLES_COUNT_DIV_2 * SAMPLES_COUNT_DIV_2);
        shadowFactor = 0.0f;
        float bias = 0.001;
        float shadowMapX, shadowMapY;
        shadowAtlas.GetDimensions(shadowMapX, shadowMapY);
        for (int i = 0; i < SAMPLES_COUNT_DIV_2; ++i)
        {
            for (int j = 0; j < SAMPLES_COUNT_DIV_2; ++j)
            {
                float2 offset = float2(i / 10, j / 10) / float(SAMPLES_COUNT_DIV_2) * INV_SAMPLES_COUNT;
                float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy + offset;
            
                sampleCoord.x = dirLightTopLeftXY.x + sampleCoord.x * dirLightViewPortSize.x;
                sampleCoord.y = dirLightTopLeftXY.y + sampleCoord.y * dirLightViewPortSize.y;
                
                // Convert to texture coordinates
                sampleCoord.x /= shadowMapX;
                sampleCoord.y /= shadowMapY;

                float computedZ = directionalLightProjectedPosition.z;
                float shadowMapZ = shadowAtlas.Sample(aDefaultSampler, sampleCoord);

                shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0 : 0.0;
            }
        }
        shadowFactor /= samples;
    }
    

    
    
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = emissiveAlbedo + directionalLight * shadowFactor + ambiance;
        
    result.color.rgb = radiance;
   
    result.color.a = 1.f;
    return result;
}

  //  if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
		//clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
  //  {
  //      float samples = float(SAMPLES_COUNT_DIV_2 * SAMPLES_COUNT_DIV_2);
  //      shadowFactor = 0.0f;
  //      float bias = 0.001;
  //      float shadowMapX, shadowMapY;
  //      directionalLightShadowMap.GetDimensions(shadowMapX, shadowMapY);
  //      for (int i = 0; i < SAMPLES_COUNT_DIV_2; ++i)
  //      {
  //          for (int j = 0; j < SAMPLES_COUNT_DIV_2; ++j)
  //          {
  //              float2 offset = float2(i / 5, j / 5) / float(SAMPLES_COUNT_DIV_2) * INV_SAMPLES_COUNT;
  //              float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy + offset;
  //              //float sampleCoordX = directionalLightProjectedPosition.x * 0.5f + 0.5f + offset.x;
  //              //float sampleCoordY = -directionalLightProjectedPosition.y * 0.5f + 0.5f + offset.y;
            
  //          // Convert to texture coordinates
  //              sampleCoord.x *= shadowMapX;
  //              sampleCoord.y *= shadowMapY;

  //              float computedZ = directionalLightProjectedPosition.z;
  //              float shadowMapZ = directionalLightShadowMap.Sample(aDefaultSampler, sampleCoord);

  //              shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0 : 0.0;
  //          }
  //      }
  //      shadowFactor /= samples;
  //  }

//if (clamp(directionalLightProjectedPosition.x, -1.0, 1.0) == directionalLightProjectedPosition.x &&
//		clamp(directionalLightProjectedPosition.y, -1.0, 1.0) == directionalLightProjectedPosition.y)
//    {
//          float samples = float(SAMPLES_COUNT_DIV_2 * SAMPLES_COUNT_DIV_2);
//        shadowFactor = 0.0f;
//          float bias = 0.001;
//        for (int i = 0;i < SAMPLES_COUNT_DIV_2; ++i)
//        {
//            for (int j = 0;j < SAMPLES_COUNT_DIV_2; ++j)
//            {
//                  float2 offset = float2(i / 5, j / 5) / float(SAMPLES_COUNT_DIV_2) * INV_SAMPLES_COUNT;
//                  float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * directionalLightProjectedPosition.xy + offset;

//                  float computedZ = directionalLightProjectedPosition.z;
//                  float shadowMapZ = directionalLightShadowMap.Sample(aDefaultSampler, sampleCoord);

//                shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0 : 0.0;
//            }
//        }
//        shadowFactor /=
//samples;
//    }