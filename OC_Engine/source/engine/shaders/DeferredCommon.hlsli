struct FullscreenVertexInput
{
    unsigned int Index : SV_VertexID;
};

struct VertexInputCube
{
    float4 Position : POSITION;
    float2 UV : TEXCOORD;
};

struct VertexToPixelCube
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : POSITION;
    //float3 viewVector : TEXCOORD1;
    float2 UV : TEXCOORD0;
};

struct FullscreenVertexToPixel
{
    float4 Position : SV_POSITION;
    float2 UV : UV;
    float3 viewVector : VIEWVECTOR;
};

struct FullscreenPixelOutput
{
    float4 color : SV_TARGET;
};

struct DeferredVertexInput
{
    float4 position : POSITION;
};

struct DeferredVertexToPixel
{
    float4 position : SV_POSITION;
};

struct DeferredVertexToPixel2
{
    float4 position : SV_POSITION;
    float4 worldPosition : TEXCOORD0;
};

static const float GaussianKernel5[5] =
{ 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136 };

Texture2D FullscreenTexture1 : register(t1);
//Texture2D FullscreenTexture2 : register(t1);
//Texture2D FullscreenTexture3 : register(t2);
//Texture2D FullscreenTexture4 : register(t3);
//SamplerState DefaultSampler : register(s0);

Texture2D gBufferPositionTexture : register(t10);
Texture2D gBufferAlbedoTexture : register(t11);
Texture2D gBufferNormalTexture : register(t12);
Texture2D gBufferMaterialTexture : register(t13);
Texture2D gBufferEffectsTexture : register(t14);

cbuffer PointLightData : register(b6)
{
    float3 lightPosition;
    float lightRange;
    float4 lightColourAndIntensity;
};

cbuffer SpotLightData : register(b7)
{
    float4x4 spotlightProjection;
    float3 spotPosition;
    float spotRange;
    float4 spotColorAndIntensity;
    float3 spotDirection;
    float paddingg1;
    float OuterLimit;
    float InnerLimit;
    float2 trashis;
    float2 spotLightViewPortSize;
    float2 spotLightTopLeftXY;
};

cbuffer CloudsBuffer : register(b8)
{
    float4 BoundsMin;
    float4 BoundsMax;
    float4 CloudOffset;
    float4 phaseParams;
    float CloudScale;
    float DensityThreshold;
    float DensityMultiplier;
    int numSteps;
    float lightAbsorptionThroughCloud;
    //float lightAbsorptionTowardSun;
    int numStepsLight;
    float darknessThreshold;
    float detailWeight;
};