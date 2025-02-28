#include "DeferredCommon.hlsli"
#include "pbr_functions.hlsli"

#define SAMPLES_COUNT 4 
#define SAMPLES_COUNT_DIV_2 2  
#define INV_SAMPLES_COUNT (1.0f / SAMPLES_COUNT)  

float4 main(DeferredVertexToPixel2 aInput) : SV_TARGET
{
    float2 uv = aInput.position.xy / resolution.zw;
    float4 worldPosition = gBufferPositionTexture.Sample(aDefaultSampler, uv.xy).xyzw;
    float3 albedo = gBufferAlbedoTexture.Sample(aDefaultSampler, uv.xy).rgb;
    float3 normal = normalize(2.f * gBufferNormalTexture.Sample(aDefaultSampler, uv.xy).xyz - 1.f);
    float4 material = gBufferMaterialTexture.Sample(aDefaultSampler, uv.xy);
    
    float3 toEye = normalize(CameraPosition.xyz - worldPosition.xyz);
    
    float1 roughness = material.g;
    float1 metalness = material.b;
    

    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, (float3) metalness);
    
    float3 spotLight = EvaluateSpotLight(
			albedo, specularColor, normal, roughness,
			spotColorAndIntensity.xyz, spotColorAndIntensity.w, spotRange, spotPosition.xyz, spotDirection.xyz,
			OuterLimit, InnerLimit,
			toEye, worldPosition.xyz);
    
    float shadowFactor = 1.0f;
  //  float4 spotlightProjectedPositionTemp = mul(spotlightProjection, aInput.worldPosition);
  //  float3 spotlightProjectedPosition = spotlightProjectedPositionTemp.xyz / spotlightProjectedPositionTemp.w;
  //  if (clamp(spotlightProjectedPosition.x, -1.0, 1.0) == spotlightProjectedPosition.x &&
		//clamp(spotlightProjectedPosition.y, -1.0, 1.0) == spotlightProjectedPosition.y)
  //  {
  //      float samples = float(SAMPLES_COUNT_DIV_2 * SAMPLES_COUNT_DIV_2);
  //      shadowFactor = 0.0f;
  //      float bias = 0.001;
  //      float shadowMapX, shadowMapY;
  //      shadowAtlas.GetDimensions(shadowMapX, shadowMapY);
  //      for (int i = 0; i < SAMPLES_COUNT_DIV_2; ++i)
  //      {
  //          for (int j = 0; j < SAMPLES_COUNT_DIV_2; ++j)
  //          {
  //              float2 offset = float2(i / 10, j / 10) / float(SAMPLES_COUNT_DIV_2) * INV_SAMPLES_COUNT;
  //              float2 sampleCoord = 0.5f + float2(0.5f, -0.5f) * spotlightProjectedPosition.xy + offset;
            
  //              sampleCoord.x = spotLightTopLeftXY.x + sampleCoord.x * spotLightViewPortSize.x;
  //              sampleCoord.y = spotLightTopLeftXY.y + sampleCoord.y * spotLightViewPortSize.y;
                
  //          // Convert to texture coordinates
  //              sampleCoord.x /= shadowMapX;
  //              sampleCoord.y /= shadowMapY;

  //              float computedZ = spotlightProjectedPosition.z;
  //              float shadowMapZ = shadowAtlas.Sample(aDefaultSampler, sampleCoord);

  //              shadowFactor += (computedZ < shadowMapZ + bias) ? 1.0 : 0.0;
  //          }
  //      }
  //      shadowFactor /= samples;
  //  }
    
    return float4(spotLight, 1.f) * shadowFactor;
}