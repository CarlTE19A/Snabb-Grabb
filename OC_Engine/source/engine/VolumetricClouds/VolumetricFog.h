#pragma once
#include "Texture3D.h"
#include <math/Transform.h>
#include <math/Vector.h>
#include <vector>

using Microsoft::WRL::ComPtr;
struct ID3D11ComputeShader;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;

class VolumetricFog
{
public:
	VolumetricFog();
	~VolumetricFog();

	void Init();
	void Update(const Vector3f aCameraPos);
	void Render();
private:
	bool IsInside(const Vector3f aCameraPos);

	Texture3D myNoiseTexture;
	Transform myTransform;
	ComPtr<ID3D11InputLayout> myInputLayout;
	ComPtr<ID3D11ComputeShader> myCSShader;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11VertexShader> myVertexShaderFullscreen;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11PixelShader> myPixelShaderFullscreen;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11Buffer> myCloudBuffer;
	unsigned int myIndexCount = 0;
	Vector4f myBoundsMin;
	Vector4f myBoundsMax;
	Vector4f myCloudOffset;
	Vector4f myPhaseParams;
	float myCloudScale;
	float myDensityThreshold;
	float myDensityMultiplier;
	int myNumSteps;
	float lightAbsorptionThroughCloud;
	int numStepsLight;
	float darknessThreshold;
	float detailWeight;

	bool isInside;

	std::vector<Vector4f> mSeedPoints;

	bool InitShadersAndBuffers();
};