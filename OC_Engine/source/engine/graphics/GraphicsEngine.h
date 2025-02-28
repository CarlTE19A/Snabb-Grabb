#pragma once
#include "GraphicsData.h"

#include "light/DirectionalLight.h"
#include "light/AmbientLight.h"
#include "GBuffer/GBuffer.h"
#include "RenderTarget.h"
#include "DepthBuffer.h"
#include "VolumetricClouds\VolumetricClouds.h"
#include "VolumetricClouds\VolumetricFog.h"

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11DepthStencilView;
struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11Buffer;
struct ID3D11DepthStencilState;
struct ID3D11BlendState;
struct D3D11_VIEWPORT;

struct Vertex;
struct MeshVertex;
class TextService;
struct Vertex;
class CameraComponent;
class SpriteDrawer;
class PointLight;
class SpotLight;
class ModelDrawer;
class FullscreenEffect;


class GraphicsEngine
{
	friend class Engine;
	struct RenderState
	{
		DirectionalLight* myDirectionalLight;
		AmbientLight myAmbientLight;
		PointLight* myPointLights[NUMBER_OF_LIGHTS_ALLOWED];
		SpotLight* mySpotLights[NUMBER_OF_LIGHTS_ALLOWED];
		int myNumberOfPointLights;
		int myNumberOfSpotLights;

		SamplerFilter mySamplerFilter = SamplerFilter::Trilinear;
		SamplerAddressMode mySamplerAddressMode = SamplerAddressMode::Wrap;
		DepthStencilState myDepthStencilState = DepthStencilState::Write;
		BlendState myBlendState = BlendState::Disabled;
		RasterizerState myRasterRizerState = RasterizerState::BackfaceCulling;
	};

public:
	inline static GraphicsEngine* GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new GraphicsEngine();

		}
		return myInstance;
	}

	inline static void DestroyInstance()
	{
		if (myInstance != nullptr)
		{
			delete myInstance;
			myInstance = nullptr;
		}
	}

	inline ID3D11DeviceContext* GetContext() { return myContext.Get(); }
	inline ComPtr<ID3D11Buffer>& GetObjectBuffer() { return myObjectBuffer; }
	inline ComPtr<ID3D11Device>& GetDevice() { return myDevice; }

	~GraphicsEngine();
	bool Initialize(int width, int height, HWND windowHandle);
	void BeginFrame();
	void UpdateGPUStates();
	void EndFrame();
	bool Resize(Vector2<int> aResolution);
	void UpdateFrameBuffer(bool aUseDepth, BlendState aBlendState = BlendState::Disabled);
	void SetClearColor(float aR, float aG, float aB)
	{
		myClearColor[0] = aR;
		myClearColor[1] = aG;
		myClearColor[2] = aB;
	}
	float* GetClearColor() { return myClearColor; }

	Vector2<int> GetWindowSize() { return myWindowSize; }
	inline Vector2f GetViewportDimensions() { return myViewport; }

	CameraComponent* GetCamera() { return myCamera; }
	CameraComponent const* GetCamera() const { return myCamera; }
	void SetCamera(CameraComponent* aCamera) { myCamera = aCamera; }

	//Lights
	void SetAmbientLight(AmbientLight& aLight);
	const AmbientLight& GetAmbientLight();
	void SetDirectionalLight(DirectionalLight* aLight);
	DirectionalLight* GetDirectionalLight();
	void AddPointLight(PointLight aPointLight);
	void ClearPointLights();
	void AddSpotLight(SpotLight aSpotLight);
	void ClearSpotLights();

	HWND& GetWindowHandle() { return *myHWND; }

	SpriteDrawer& GetSpriteDrawer() { return *mySpriteDrawer; }

	void SetEngineBackBufferAsActiveTarget(bool aUseDepth);
	inline void SetDepthStencilState(const DepthStencilState aDepthStencilState) { myRenderstate.myDepthStencilState = aDepthStencilState; }
	inline ID3D11BlendState& GetBlendState(const BlendState aBlendState) { return *myBlendStates[static_cast<int>(aBlendState)].Get(); }
	inline void SetBlendState(const BlendState aBlendState) { myRenderstate.myBlendState = aBlendState; }
	inline void SetRasterizerState(const RasterizerState aRasterizerState) { myRenderstate.myRasterRizerState = aRasterizerState; }
	void SetSamplerState(const SamplerFilter aFilter, const SamplerAddressMode aAddressMode);
	void UseDepth(bool aUseDepth);
	ModelDrawer& GetModelDrawer() { return *myModelDrawer; }
	TextService& GetTextService() { return *myTextService; }
	
	FullscreenEffect& GetFullscreenDeferred() { return *myDeferredRendering; }
	FullscreenEffect& GetFullscreenVingette() { return *myVingetteRendering; }
	FullscreenEffect& GetFullscreenSpeedLines() { return *mySpeedLinesRendering; }
	FullscreenEffect& GetFullscreenComic() { return *myComicRendering; }
	FullscreenEffect& GetFullScreenEffectUpSampleAndScaleBloom() { return *myFullscreenUpSampleAndScaleEffect; }
	FullscreenEffect& GetFullScreenEffectDownSample() { return *myFullscreenDownSampleEffect; }
	FullscreenEffect& GetFullScreenEffectCopy() { return *myFullscreenCopy; }
	FullscreenEffect& GetFullScreenPostprocess() { return *myFullscreenPostprocess; }

	GBuffer& GetGBuffer() { return myGbuffer; }

	//ID3D11DepthStencilView& GetDepthBuffer() { return *myDepthBuffer.Get(); }
	DepthBuffer* GetDepthBuffer() { return DepthBufferptr; }
	RenderTarget* GetBackBuffer() { return BackBufferptr; }
	void SetFullScreen(bool aFullscreen);
	bool CreateBuffer(uint32_t aSize, ComPtr<ID3D11Buffer>& aBuffer);
	void ReadPostprocessSettings(const int aPostProcessIndex);
	void SavePostprocessSettings(const char* aPath);

	inline float& GetBloomBlending() { return myBloomBlending; }
	inline float& GetSaturation() { return mySaturation; }
	inline float& GetExposure() { return myExposure; }
	inline Vector3f& GetBlackPoint() { return myBlackpoint; }
	inline Vector3f& GetTint() { return myTint; }
	inline Vector3f& GetContrast() { return myContrast; }
	
	void RenderVolumetricClouds();
	void RenderVolumetricFog();

	void UpdateClouds();
	void UpdateFog();

	std::vector<RenderTarget>& GetBloomRenderTargets() { return myBloomRenderTargets; }
	DepthBuffer& GetShadowMap() { return myShadowAtlas; }
	//only to be called before loading a new level, to allow new lights to register for a spot in the atlas
	void ClearShadowAtlas();
	RenderTarget& GetComicShaderRenderTarget() { return myComicShaderRenderTarget; }
private:
	GraphicsEngine();

	bool CreateBlendStates();
	bool CreateDepthStencilStates();
	bool CreateRasterizerStates();
	bool CreateSamplers();
	bool CreateConstantBuffers();


	std::array<ComPtr<ID3D11DepthStencilState>, (int)DepthStencilState::Count> myDepthStencilStates;
	std::array<ComPtr<ID3D11BlendState>, (int)BlendState::Count> myBlendStates;
	std::array<ComPtr<ID3D11RasterizerState>, (int)RasterizerState::Count> myRasterizerStates;
	std::array<std::array<ComPtr<ID3D11SamplerState>, (int)SamplerAddressMode::Count>, (int)SamplerFilter::Count> mySamplerStates;


	ComPtr<ID3D11Device> myDevice;
	ComPtr<ID3D11DeviceContext> myContext;
	ComPtr<IDXGISwapChain> mySwapChain;
	ComPtr<ID3D11Buffer> myFrameBuffer;
	ComPtr<ID3D11Buffer> myObjectBuffer;
	ComPtr<ID3D11Buffer> myCameraBuffer;
	ComPtr<ID3D11Buffer> myLightBuffer;
	ComPtr<ID3D11Buffer> myPostProcessBuffer;

	RenderTarget* BackBufferptr;
	DepthBuffer* DepthBufferptr;
	RenderTarget myBackBuffer;
	DepthBuffer myDepthBuffer;
	
	DepthBuffer myShadowAtlas;
	RenderTarget myComicShaderRenderTarget;
	std::vector<RenderTarget> myBloomRenderTargets;

	RenderState myRenderstate;
	//constant buffers

	SpriteDrawer* mySpriteDrawer;
	TextService* myTextService;
	CameraComponent* myCamera;
	HWND* myHWND;
	float myClearColor[4];
	//postprocess values
	Vector3f myContrast;
	Vector3f myTint;
	Vector3f myBlackpoint;
	float mySaturation;
	float myBloomBlending;
	float myExposure;

	static GraphicsEngine* myInstance;

	Vector2f myViewport;
	Vector2<int> myWindowSize;

	ModelDrawer* myModelDrawer;

	std::unique_ptr<FullscreenEffect> myDeferredRendering;
	std::unique_ptr<FullscreenEffect> myVingetteRendering;
	std::unique_ptr<FullscreenEffect> mySpeedLinesRendering;
	std::unique_ptr<FullscreenEffect> myComicRendering;
	std::unique_ptr<FullscreenEffect> myFullscreenDownSampleEffect;
	std::unique_ptr<FullscreenEffect> myFullscreenUpSampleAndScaleEffect;
	std::unique_ptr<FullscreenEffect> myFullscreenCopy;
	std::unique_ptr<FullscreenEffect> myFullscreenPostprocess;
	//D3D11_VIEWPORT* myActualViewPort;

	GBuffer myGbuffer;

	VolumetricClouds myVolumetricClouds;
	VolumetricFog myVolumetricFog;

	//for postprocess
	char* myPostProccessArray[3];
	char* myCurrentPostProcessingPath;

	friend class EditorState;
};
