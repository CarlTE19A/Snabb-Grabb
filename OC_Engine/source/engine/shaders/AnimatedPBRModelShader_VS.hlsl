#include "common.hlsli"

float3x3 invertMatrix(float3x3 m)
{
	// computes the inverse of a matrix m
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    float invdet = 1 / det;

    float3x3 minv; // inverse of matrix m
    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

    return minv;
}

PixelInputType main(VertexInputType input)
{
    PixelInputType result;
    float4 vertexObjectPos = input.position;
    
    float4x4 skinnedMatrix = 0;
    uint iBone = 0;
    float fWeight = 0;

	// Bone 0
    iBone = input.boneIndices.x;
    fWeight = input.weights.x;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 1
    iBone = input.boneIndices.y;
    fWeight = input.weights.y;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 2
    iBone = input.boneIndices.z;
    fWeight = input.weights.z;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 3
    iBone = input.boneIndices.w; 
    fWeight = input.weights.w;
    skinnedMatrix += fWeight * Bones[iBone];
    
    float4 vertexWorldPos = mul(modelToWorld, mul(skinnedMatrix, vertexObjectPos));
    float4 vertexViewPos = mul(worldToCameraSpace, vertexWorldPos);
    float4 vertexProjectionPos = mul(cameraToProjection, vertexViewPos);
   
    float3x3 skinnedRotation = (float3x3)skinnedMatrix;
    float3x3 toWorldRotation = (float3x3)modelToWorld;
	//float3 vertexWorldNormal = mul(toWorldRotation, input.normal);

    float3 vertexWorldBinormal = mul(toWorldRotation, mul(skinnedRotation, input.binormal));
    float3 vertexWorldTangent = mul(toWorldRotation, mul(skinnedRotation, input.tangent));
    float3 vertexWorldNormal = mul(transpose(invertMatrix(toWorldRotation)), mul(transpose(invertMatrix(skinnedRotation)), input.normal));

    result.position = vertexProjectionPos;
    result.worldPosition = vertexWorldPos;
    result.color = input.color;

    result.uv = input.uv;

    result.normal = vertexWorldNormal;
    result.binormal = vertexWorldBinormal;
    result.tangent = vertexWorldTangent;
    
    return result;
}
