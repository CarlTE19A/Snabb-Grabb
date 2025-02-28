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

class VolumetricClouds
{
public:
	VolumetricClouds();
	~VolumetricClouds();

	void Init();
	void Update(const Vector3f aFollowPos);
	void Render();
private:
	Texture3D myNoiseTexture;
	Transform myTransform;
	ComPtr<ID3D11InputLayout> myInputLayout;
	ComPtr<ID3D11ComputeShader> myCSShader;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11Buffer> mySeedBuffer;
	ComPtr<ID3D11Buffer> myCloudBuffer;
	unsigned int myIndexCount = 0;
	//You stupid
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

	std::vector<Vector4f> mSeedPoints;

	bool InitShadersAndBuffers();
};