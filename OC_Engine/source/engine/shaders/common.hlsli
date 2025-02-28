#define NUMBER_OF_LIGHTS_ALLOWED 8 
#define MAX_ANIMATION_BONES 64 

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0;
    int iheight = 0;
    int numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iheight, numMips);
    return numMips;
}

struct VertexInputType //TODO: check if skinned meshes will need another vertex struct
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
    float4 boneIndices : BONES;
    float4 weights : WEIGHTS;
    
    float4x4 transform : INSTANCE_TRANSFORM;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 binormal : BINORMAL;
    float3 tangent : TANGENT;
};

struct PixelOutput
{
    float4 color : SV_TARGET;
};

struct GBufferOutput
{
    float4 myWorldPosition : SV_TARGET0;
    float4 myAlbedo : SV_TARGET1;
    float4 myNormal : SV_TARGET2;
    float4 myMaterial : SV_TARGET3;
    float4 myEffectsAndCustom : SV_TARGET4;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 worldToCameraSpace;
    float4x4 cameraToProjection;
    float4x4 invCameraToProjection;
    float4 resolution; //myResolution.x = screen width, myResolution.y = screen height, myResolution.z = unset, myResolution.w = unset
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 modelToWorld;
    float4 color;
    int highlight;
    int pad1;
    int pad2;
    int pad3;
}

cbuffer CameraBuffer : register(b2)
{
    float4 CameraPosition;
    float NearPlane;
    float FarPlane;
    float time;
    float unused0;
    float4 CameraForward;
    float4 CameraRight;
    float4 CameraUp;
}

cbuffer PostProcessConstantBufferData : register(b10)
{
    float3 Contrast;
    float BloomBlending;
    float3 Tint;
    float Saturation;
    float3 Blackpoint;
    float Exposure;
}

cbuffer LightBuffer : register(b3)
{
    //struct PointLightData
    //{
    //    float4 Position;
    //    float4 Color;
    //    float Range;
    //    float3 padding;
    //} myPointLights[NUMBER_OF_LIGHTS_ALLOWED];
    
    //struct SpotLightData
    //{
    //    float4 Position;
    //    float4 Color;
    //    float4 Direction;
    //    float Range;
    //    float OuterLimit;
    //    float InnerLimit;
    //    float garbage; // Padding, don't use arrays!
    //} mySpotLights[NUMBER_OF_LIGHTS_ALLOWED];
    
    //uint NumberOfPointLights;
    //uint NumberOfSpotLights;
    //float2 oopadding0;
    
    float4x4 dirLightProjection;
    float4 dirLightPosition;
    float4 dLightColorAndIntensity;
    float3 dLightDir;
    float padding0;
    float2 dirLightViewPortSize;
    float2 dirLightTopLeftXY;
}

cbuffer BoneBuffer : register(b4)
{
    float4x4 Bones[MAX_ANIMATION_BONES];
};

cbuffer VFXBufferData : register(b5)
{
    float vfxTime;
    float vfxIntencity;
    float2 padding5;
};

cbuffer ParticleBuffer : register(b6)
{
    float3 InitialPos;
    float3 InitialVelocity;
    float2 Size;
    float Age;
};

struct ParticleData
{
    float3 InitialPos;
    float3 InitialVelocity;
    float2 Size;
    float Age;
};

cbuffer SpeedLinesBuffer : register(b9)
{
    float3 speedLinesDirection;
    float1 speedLinesSpeed;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
    float1 value : VALUE;
};

float GetLogDepth(float4 worldPosition)
{
    float4 cameraSpacePos = mul(worldToCameraSpace, worldPosition);
    float4 projectionPos = mul(cameraToProjection, cameraSpacePos);
    return projectionPos.z / projectionPos.w;
}

float GetLinDepth(float4 worldPosition)
{
    float logDepth = GetLogDepth(worldPosition);
    return NearPlane / (FarPlane - logDepth * (FarPlane - NearPlane));
}

SamplerState aDefaultSampler : register(s0);

TextureCube aEnvironmentTexture : register(t0);
Texture2D aAlbedoTexture : register(t1);
Texture2D aNormalTexture : register(t2);
Texture2D aMaterialTexture : register(t3);
Texture2D aEffectsTexture : register(t4);
Texture2D aDepthTexture : register(t5);
Texture2D perlinTexture : register(t6);

RWTexture3D<float4> cOutPut : register(u1);
RWTexture3D<float4> cOutPutPerlin : register(u0);
Texture3D worleyNoise : register(t7);
Texture3D perlinNoise : register(t8);
SamplerState shadowSampler : register(s8);
Texture2D shadowAtlas : register(t8);
TextureCube skyBox : register(t9);
