#pragma once
#include <wrl/client.h>
#include <DirectXMath.h>
//#include "../math/Vector.h"

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

#define NUMBER_OF_LIGHTS_ALLOWED 8


struct FrameBufferData
{
	DirectX::XMMATRIX worldToClipMatrix;
	DirectX::XMMATRIX cameraToProjection;
	DirectX::XMMATRIX invCameraToProjection;
	Vector4<float> resolution; //myResolution.x = screen width, myResolution.y = screen height, myResolution.z = unset, myResolution.w = unset
};

struct ObjectBufferData
{
	DirectX::XMMATRIX modelToWorld;
	Vector4f color;
	int isHighlight;
	int pad1;
	int pad2;
	int pad3;
};

struct CameraBufferData
{
	Vector4<float> cameraPosition;
	float nearPlane;
	float farPlane;
	float time;
	float BloomBlending;
	Vector4f cameraForward;
	Vector4f cameraRIght;
	Vector4f cameraUp;
};

struct PostProcessConstantBufferData
{
	Vector3f Contrast;
	float BloomBlending;
	Vector3f Tint;
	float Saturation;
	Vector3f Blackpoint;
	float Exposure;
};

//struct ParticleBufferData
//{
//	DirectX::XMFLOAT3 InitialPos;
//	DirectX::XMFLOAT3 InitialVel;
//	DirectX::XMFLOAT2 Size;
//	float Age;
//	Vector2f Garbage;
//};


struct LightBufferData
{
	/*struct PointLightData
	{
		Vector4f myPosition;
		Vector4f myColorAndIntensity;
		float myRange;
		Vector3f padding;
	} myPointLights[NUMBER_OF_LIGHTS_ALLOWED];

	struct SpotLightData
	{
		Vector4f myPosition;
		Vector4f myColorAndIntensity;
		Vector4f myDirection;
		float myRange;
		float myOuterLimit;
		float myInnerLimit;
		float padding;
	} mySpotLights[NUMBER_OF_LIGHTS_ALLOWED];

	unsigned int myNumberOfPointLights;
	unsigned int myNumberOfSpotLights;
	Vector2f padding0;*/

	DirectX::XMMATRIX directionLightProjection;
	Vector4<float> dLightPosition;
	Vector4<float> dLightColorAndIntensity;
	Vector3<float> dLightDir;
	float padding1;
	Vector2f dirLightViewPortSize;
	Vector2f dirLightTopLeftXY;
};

struct VFXBufferData
{
	float vfxTime;
	float vfxIntencity;
	Vector2f padding0;
};

enum class DepthStencilState
{
	Write,
	ReadOnly,
	ReadOnlyGreater,
	ReadOnlyLess,
	ReadLessEqual,
	ReadGreaterEqual,
	Disabled,
	Count
};

enum class BlendState
{
	Disabled,
	AlphaBlend,
	AdditiveBlend,
	AddAndFadeBackground,
	Count
};

enum class RasterizerState
{
	BackfaceCulling,
	FrontCulling,
	NoFaceCulling,
	Wireframe,
	WireframeNoCulling,
	FrontfaceCulling,
	Count,
};

enum class SamplerFilter
{
	Point,
	Bilinear,
	Trilinear,
	Count
};

enum class SamplerAddressMode
{
	Clamp,
	Wrap,
	Mirror,
	Count
};