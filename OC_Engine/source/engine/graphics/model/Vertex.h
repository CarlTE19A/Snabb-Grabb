#pragma once
//#include "math/Vector.h"

struct MeshVertex
{
	Vector4<float> position;
	Vector4<float> color;
	Vector2<float> texCoord;
	Vector3<float> normal;
	Vector3<float> binormal;
	Vector3<float> tangent;
};

struct AnimatedMeshVertex
{
	Vector4<float> position;
	Vector4<float> color;
	Vector2<float> texCoord;
	Vector3<float> normal;
	Vector3<float> binormal;
	Vector3<float> tangent;

	float BoneIDs[4] = { 0, 0, 0, 0 };
	float BoneWeights[4] = { 0, 0, 0, 0 };
};

struct SimpleVertex
{
	float x, y, z;
	float r, g, b, a;
};

struct SimplestVertex
{
	float x, y, z, w;
};

struct SpecialVertex
{
	float x, y, z, w;
	float u, v;
};