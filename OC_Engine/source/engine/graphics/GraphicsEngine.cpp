#include "stdafx.h"

#include "GraphicsEngine.h"
#include "Component/CameraComponent.h"
#include "math/Timer.h"
#include "math/helper.h"
#include <cassert>
#include "graphics\sprite\SpriteDrawer.h"
#include "../Engine.h"
#include <ApplicationWindow\ApplicationWindow.h>
#include "Text\TextService.h"
#include "gameObject\GameObject.h"

#include "light/AmbientLight.h"
#include "light/PointLight.h"
#include "light/SpotLight.h"

#include "model\ModelDrawer.h"
#include "FullscreenEffect.h"

#include <fstream>
#include "nlohmann\json.hpp"

#define REPORT_DX_WARNINGS

GraphicsEngine* GraphicsEngine::myInstance = nullptr;

void GraphicsEngine::SetDirectionalLight(DirectionalLight* aLight)
{
	myRenderstate.myDirectionalLight = aLight;
}

DirectionalLight* GraphicsEngine::GetDirectionalLight()
{
	return myRenderstate.myDirectionalLight;
}

void GraphicsEngine::SetAmbientLight(AmbientLight& aLight)
{
	myRenderstate.myAmbientLight = aLight;
}

const AmbientLight& GraphicsEngine::GetAmbientLight()
{
	return myRenderstate.myAmbientLight;
}

void GraphicsEngine::AddPointLight(PointLight aPointLight)
{
	assert(myRenderstate.myNumberOfPointLights < NUMBER_OF_LIGHTS_ALLOWED);

	myRenderstate.myPointLights[myRenderstate.myNumberOfPointLights] = &aPointLight;
	myRenderstate.myNumberOfPointLights++;
}

void GraphicsEngine::ClearPointLights()
{
	myRenderstate.myNumberOfPointLights = 0;
}

void GraphicsEngine::AddSpotLight(SpotLight aSpotLight)
{
	assert(myRenderstate.myNumberOfSpotLights < NUMBER_OF_LIGHTS_ALLOWED);

	myRenderstate.mySpotLights[myRenderstate.myNumberOfSpotLights] = &aSpotLight;
	myRenderstate.myNumberOfSpotLights++;
}

void GraphicsEngine::ClearSpotLights()
{
	myRenderstate.myNumberOfSpotLights = 0;
}

void GraphicsEngine::SetEngineBackBufferAsActiveTarget(bool aUseDepth)
{
	if (aUseDepth)
	{
		//myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
		BackBufferptr->SetAsActiveTarget(DepthBufferptr);
	}
	else
	{
		//myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);
		BackBufferptr->SetAsActiveTarget();
	}
	//myContext->RSSetViewports(1, myActualViewPort);
	//myContext->ClearRenderTargetView(myBackBuffer.Get(), myClearColor);
	//myContext->ClearDepthStencilView(myDepthBuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsEngine::SetSamplerState(const SamplerFilter aFilter, const SamplerAddressMode aAddressMode)
{
	myRenderstate.mySamplerFilter = aFilter;
	myRenderstate.mySamplerAddressMode = aAddressMode;
}

void GraphicsEngine::UseDepth(bool aUseDepth)
{
	if (aUseDepth)
	{
		//myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), myDepthBuffer.Get());
		BackBufferptr->SetAsActiveTarget(DepthBufferptr);
	}
	else
	{
		//myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), nullptr);
		BackBufferptr->SetAsActiveTarget();
	}
}

void GraphicsEngine::SetFullScreen(bool aFullscreen)
{
	mySwapChain->SetFullscreenState(aFullscreen, nullptr);
	LogInfo("Set Fullscreen to: " + std::to_string(aFullscreen));
}

void GraphicsEngine::RenderVolumetricClouds()
{
	myVolumetricClouds.Render();
}

void GraphicsEngine::RenderVolumetricFog()
{
	myVolumetricFog.Render();
}

void GraphicsEngine::UpdateClouds()
{
	if(myCamera->GetOwner() !=nullptr)
	{
		myVolumetricClouds.Update(myCamera->GetOwner()->GetPosition());
	}
}

void GraphicsEngine::UpdateFog()
{
	if (myCamera->GetOwner() != nullptr)
	{
		myVolumetricFog.Update(myCamera->GetOwner()->GetPosition());
	}
}

void GraphicsEngine::ClearShadowAtlas()
{
	myShadowAtlas.Clear();
	myShadowAtlas.myViewports.clear();
}

GraphicsEngine::GraphicsEngine()
	:DepthBufferptr(nullptr)
	, BackBufferptr(nullptr)
{
	myClearColor[0] = 0.f;
	myClearColor[1] = 0.f;
	myClearColor[2] = 0.f;
	myClearColor[3] = 1.f;
}


GraphicsEngine::~GraphicsEngine()
{
	delete mySpriteDrawer;
	delete myTextService;
	delete myModelDrawer;

	for (int i = 0; i < myDepthStencilStates.size(); ++i)
	{
		if (myDepthStencilStates[i])
			myDepthStencilStates[i].Reset();
	}
	for (int i = 0; i < myBlendStates.size(); ++i)
	{
		if (myBlendStates[i])
			myBlendStates[i].Reset();
	}
	for (int i = 0; i < myRasterizerStates.size(); ++i)
	{
		if (myRasterizerStates[i])
			myRasterizerStates[i].Reset();
	}
	for (int i = 0; i < mySamplerStates.size(); ++i)
	{
		for (int j = 0; j < mySamplerStates[i].size(); ++j)
		{
			if (mySamplerStates[i][j])
				mySamplerStates[i][j].Reset();
		}
	}

	myDevice.Reset();
	myContext.Reset();
	mySwapChain.Reset();
	myFrameBuffer.Reset();
	myObjectBuffer.Reset();
	myCameraBuffer.Reset();
	myLightBuffer.Reset();
}

bool GraphicsEngine::Initialize(int width, int height, HWND windowHandle)
{
	myWindowSize = Vector2<int>(width, height);

	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.Windowed = true;
	UINT creationFlags = 0;
#if defined(REPORT_DX_WARNINGS)
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags, nullptr, 0,
		D3D11_SDK_VERSION, &swapChainDesc, &mySwapChain, &myDevice, nullptr, &myContext);

	ID3D11Texture2D* backBufferTexture;
	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
	if (FAILED(result)) { return false; }
	myBackBuffer = RenderTarget::Create(backBufferTexture);
	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myDepthBuffer = DepthBuffer::Create(Vector2<int>((int)textureDesc.Width, (int)textureDesc.Height));
	myBackBuffer.SetAsActiveTarget(&myDepthBuffer);

	DepthBufferptr = &myDepthBuffer;
	BackBufferptr = &myBackBuffer;
	myViewport = Vector2f((float)Engine::GetInstance()->GetApplicationWindow().GetWidth(), (float)Engine::GetInstance()->GetApplicationWindow().GetHeight());
	CreateConstantBuffers();
	CreateDepthStencilStates();
	CreateBlendStates();
	CreateRasterizerStates();
	CreateSamplers();
	BackBufferptr->SetAsActiveTarget(DepthBufferptr);


	myHWND = &windowHandle;
	myModelDrawer = new ModelDrawer();
	if (!myModelDrawer->Init())
	{
		return false;
	}
	mySpriteDrawer = new SpriteDrawer();
	mySpriteDrawer->Init(myContext);

	myTextService = new TextService();
	myTextService->Init();

	myDeferredRendering = std::make_unique<FullscreenEffect>();
	if (!myDeferredRendering->Init("FullscreenDeferred_PS.cso"))
	{
		return false;
	}

	myVingetteRendering = std::make_unique<FullscreenEffect>();
	if (!myVingetteRendering->Init("FullscreenVingette_PS.cso"))
	{
		return false;
	}

	mySpeedLinesRendering = std::make_unique<FullscreenEffect>();
	if (!mySpeedLinesRendering->Init("FullscreenSpeedLines_PS.cso"))
	{
		return false;
	}

	myComicRendering = std::make_unique<FullscreenEffect>();
	if (!myComicRendering->Init("FullscreenDeferredComic_PS.cso"))
	{
		return false;
	}

	myFullscreenUpSampleAndScaleEffect = std::make_unique<FullscreenEffect>();
	if (!myFullscreenUpSampleAndScaleEffect->Init("PostProcessUpSampleAndScaleBloom_PS.cso"))
	{
		return false;
	}

	myFullscreenDownSampleEffect = std::make_unique<FullscreenEffect>();
	if (!myFullscreenDownSampleEffect->Init("PostProcessDownSample_PS.cso"))
	{
		return false;
	}

	myFullscreenCopy = std::make_unique<FullscreenEffect>();
	if (!myFullscreenCopy->Init("FullscreenCopy_PS.cso"))
	{
		return false;
	}

	myFullscreenPostprocess = std::make_unique<FullscreenEffect>();
	if (!myFullscreenPostprocess->Init("PostProcessMixWithTonemap_PS.cso"))
	{
		return false;
	}

	myBloomRenderTargets.resize(6);
	int scalar = 2;
	Vector2<int> viewPort = { (int)GetViewportDimensions().x, (int)GetViewportDimensions().y };
	myBloomRenderTargets[0] = RenderTarget::Create(viewPort, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	for (int i = 1; i < myBloomRenderTargets.size(); i++)
	{
		myBloomRenderTargets[i] = RenderTarget::Create(viewPort / scalar, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		scalar *= 2;
	}
	myShadowAtlas = DepthBuffer::CreateForShadowAtlas({ 16384, 16384 });
	myComicShaderRenderTarget = RenderTarget::Create({ (int)GetViewportDimensions().x,(int)GetViewportDimensions().y }, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	myGbuffer = GBuffer::Create(Vector2<int>(Vector2<int>((int)textureDesc.Width, (int)textureDesc.Height)));
	myBloomBlending = 0.5f;
	myContrast = { 1,1,1 };
	myTint = { 1,1,1 };
	myBlackpoint = {};
	mySaturation = 1.f;
	myExposure = 0.f;

	myVolumetricClouds.Init();
	myVolumetricFog.Init();

	myPostProccessArray[0] = "Postprocess_1.json";
	myPostProccessArray[1] = "Postprocess_2.json";
	myPostProccessArray[2] = "Postprocess_3.json";
	myCurrentPostProcessingPath = myPostProccessArray[0];
	ReadPostprocessSettings(0);
	return true;
}

void GraphicsEngine::BeginFrame()
{
	if (myCamera == nullptr)
	{
		assert(false && "Engine Camera is nullptr");
		return;
	}
#ifdef _DEBUG
	if (GetAsyncKeyState('P'))
	{
		myComicRendering->RecompileShader(myDevice.Get(), L"../source/engine/shaders/FullscreenDeferredComic_PS.hlsl");
	}
#endif // DEBUG

	myBackBuffer.Clear(myClearColor);
	myDepthBuffer.Clear(1.0f, 0);
	BackBufferptr->SetAsActiveTarget(DepthBufferptr);

	for (int i = 0; i < myBloomRenderTargets.size(); i++)
	{
		myBloomRenderTargets[i].Clear(myClearColor);
	}
	myShadowAtlas.Clear();
	myComicShaderRenderTarget.Clear(myClearColor);
	UpdateGPUStates();
}

void GraphicsEngine::UpdateGPUStates()
{
	myContext->OMSetBlendState(myBlendStates[(int)myRenderstate.myBlendState].Get(), nullptr, 0xffffffff);
	if (myRenderstate.myDepthStencilState == DepthStencilState::Disabled)
	{
		myContext->OMSetDepthStencilState(nullptr, 0);
	}
	else
	{
		myContext->OMSetDepthStencilState(myDepthStencilStates[(int)myRenderstate.myDepthStencilState].Get(), 0);
	}
	myContext->RSSetState(myRasterizerStates[(int)myRenderstate.myRasterRizerState].Get());
	myContext->PSSetSamplers(0, 1, mySamplerStates[(int)myRenderstate.mySamplerFilter][(int)myRenderstate.mySamplerAddressMode].GetAddressOf());
	myContext->CSSetSamplers(0, 1, mySamplerStates[(int)myRenderstate.mySamplerFilter][(int)myRenderstate.mySamplerAddressMode].GetAddressOf());
	//myContext->CSSetSamplers(0, 1, mySamplerStates[(int)myRenderstate.mySamplerFilter][(int)myRenderstate.mySamplerAddressMode].GetAddressOf());
	myContext->OMSetBlendState(myBlendStates[(int)myRenderstate.myBlendState].Get(), nullptr, 0xffffffff);

	{
		// Prepare FrameBufferData
		FrameBufferData frameBufferData = {};
		frameBufferData.worldToClipMatrix = myCamera->GetModelMatrix();
		frameBufferData.cameraToProjection = myCamera->GetProjectionMatrix();
		frameBufferData.invCameraToProjection = DirectX::XMMatrixInverse(nullptr, myCamera->GetProjectionMatrix());
		frameBufferData.resolution = { (float)myWindowSize.x, (float)myWindowSize.y, myViewport.x, myViewport.y };
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));
		myContext->Unmap(myFrameBuffer.Get(), 0);
		myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
		myContext->CSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	}
	{
		CameraBufferData cameraBufferData = {};
		cameraBufferData.cameraPosition = Vector4f(myCamera->GetOwner()->GetPosition(), 1.0f);
		cameraBufferData.nearPlane = myCamera->GetNearPlane();
		cameraBufferData.farPlane = myCamera->GetFarPlane();
		cameraBufferData.time = (float)Engine::GetInstance()->GetTimer()->GetTotalTime();
		cameraBufferData.cameraForward = Vector4f(myCamera->GetForward().x, myCamera->GetForward().y, myCamera->GetForward().z, 0);
		cameraBufferData.cameraRIght = Vector4f(myCamera->GetRight().x, myCamera->GetRight().y, myCamera->GetRight().z, 0);
		cameraBufferData.cameraUp = Vector4f(myCamera->GetUp().x, myCamera->GetUp().y, myCamera->GetUp().z, 0);

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myCameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &cameraBufferData, sizeof(CameraBufferData));
		myContext->Unmap(myCameraBuffer.Get(), 0);
		myContext->VSSetConstantBuffers(2, 1, myCameraBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(2, 1, myCameraBuffer.GetAddressOf());
	}
	{
		PostProcessConstantBufferData postprocessData = {};
		postprocessData.Blackpoint = myBlackpoint;
		postprocessData.BloomBlending = myBloomBlending;
		postprocessData.Contrast = myContrast;
		postprocessData.Exposure = myExposure;
		postprocessData.Saturation = mySaturation;
		postprocessData.Tint = myTint;
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myPostProcessBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &postprocessData, sizeof(PostProcessConstantBufferData));
		myContext->Unmap(myPostProcessBuffer.Get(), 0);
		myContext->PSSetConstantBuffers(10, 1, myPostProcessBuffer.GetAddressOf());
	}
	{
		// Input Data
		LightBufferData lightData = {};
		if (myRenderstate.myDirectionalLight != nullptr)
		{
			lightData.dLightDir = myRenderstate.myDirectionalLight->GetTransform().GetForward();
			lightData.dLightColorAndIntensity = Vector4f(myRenderstate.myDirectionalLight->GetTransform().GetPosition(), 0);
			lightData.dLightColorAndIntensity = myRenderstate.myDirectionalLight->GetColor();
			lightData.dLightColorAndIntensity.w = myRenderstate.myDirectionalLight->GetIntensity();
			lightData.dirLightViewPortSize = myRenderstate.myDirectionalLight->myViewPortSize;
			lightData.dirLightTopLeftXY = myRenderstate.myDirectionalLight->myTopLeftXY;
			lightData.directionLightProjection = DirectX::XMMatrixInverse(nullptr, myRenderstate.myDirectionalLight->GetTransform().GetMatrix()) * myRenderstate.myDirectionalLight->GetLightProjectionMatrix();
		}
		else
		{
			lightData.dLightColorAndIntensity = Vector4f(0, 0, 0, 0);
		}
		////pointlights data
		//for (int i = 0; i < myRenderstate.myNumberOfPointLights; i++)
		//{
		//	const PointLight& pointLight = *myRenderstate.myPointLights[i];
		//	lightData.myPointLights[i].myColorAndIntensity = pointLight.GetColor();
		//	lightData.myPointLights[i].myColorAndIntensity.w = pointLight.GetIntensity();
		//	lightData.myPointLights[i].myRange = pointLight.GetRange();
		//	lightData.myPointLights[i].myPosition = Vector4f(pointLight.GetTransform().GetPosition().x,
		//		pointLight.GetTransform().GetPosition().y,
		//		pointLight.GetTransform().GetPosition().z,
		//		0.f);
		//}
		//lightData.myNumberOfPointLights = myRenderstate.myNumberOfPointLights;

		////spotlights data
		//for (int i = 0; i < myRenderstate.myNumberOfSpotLights; i++)
		//{
		//	const SpotLight& spotLight = *myRenderstate.mySpotLights[i];
		//	lightData.mySpotLights[i].myColorAndIntensity = spotLight.GetColor();
		//	lightData.mySpotLights[i].myColorAndIntensity.w = spotLight.GetIntensity();
		//	lightData.mySpotLights[i].myRange = spotLight.GetRange();
		//	lightData.mySpotLights[i].myPosition = Vector4f(spotLight.GetTransform().GetPosition().x,
		//		spotLight.GetTransform().GetPosition().y,
		//		spotLight.GetTransform().GetPosition().z,
		//		0.f);
		//	lightData.mySpotLights[i].myInnerLimit = spotLight.GetInnerLimit();
		//	lightData.mySpotLights[i].myOuterLimit = spotLight.GetOuterLimit();
		//	lightData.mySpotLights[i].myDirection = Vector4f(
		//		spotLight.GetTransform().GetForward().x,
		//		spotLight.GetTransform().GetForward().y,
		//		spotLight.GetTransform().GetForward().z,
		//		0.f);
		//}
		//lightData.myNumberOfSpotLights = myRenderstate.myNumberOfSpotLights;

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		myContext->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &lightData, sizeof(LightBufferData));
		myContext->Unmap(myLightBuffer.Get(), 0);

		myContext->VSSetConstantBuffers(3, 1, myLightBuffer.GetAddressOf());
		myContext->PSSetConstantBuffers(3, 1, myLightBuffer.GetAddressOf());
	}
	myContext->PSSetShaderResources(0, 1, myRenderstate.myAmbientLight.GetCubemap());
}

void GraphicsEngine::EndFrame()
{
	mySwapChain->Present(0, 0);
}

bool GraphicsEngine::Resize(Vector2<int> aResolution)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	myContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
	myContext->OMSetDepthStencilState(0, 0);
	myContext->ClearState();

	if (!mySwapChain)
	{
		return false;
	}

	myBackBuffer = RenderTarget();
	myDepthBuffer = DepthBuffer();

	myContext->Flush();
	if (mySwapChain->ResizeBuffers(0, aResolution.x, aResolution.y, DXGI_FORMAT_UNKNOWN, 0) != S_OK)
	{
		LogError("Could not resize buffers!");
		return false;
	}

	BackBufferptr = nullptr;
	DepthBufferptr = nullptr;

	ID3D11Texture2D* backBufferTexture = nullptr;
	if (mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture) != S_OK)
	{
		LogError("Could not resize buffers!");
		return false;
	}

	if (!backBufferTexture)
	{
		return false;
	}

	myBackBuffer = RenderTarget::Create(backBufferTexture);
	D3D11_TEXTURE2D_DESC textureDesc;
	backBufferTexture->GetDesc(&textureDesc);
	backBufferTexture->Release();
	myDepthBuffer = DepthBuffer::Create({ static_cast<int>(textureDesc.Width), static_cast<int>(textureDesc.Height) });
	myBackBuffer.SetAsActiveTarget(&myDepthBuffer);
	myViewport = Vector2<float>((float)textureDesc.Width, (float)textureDesc.Height);
	//myWindowSize = Vector2i((int)myViewport.x, (int)myViewport.y);
	myGbuffer = GBuffer::Create(Vector2<int>((int)textureDesc.Width, (int)textureDesc.Height));
	BackBufferptr = &myBackBuffer;
	DepthBufferptr = &myDepthBuffer;

	myBloomRenderTargets.clear();
	myBloomRenderTargets.resize(6);
	int scalar = 2;
	Vector2<int> viewPort = { (int)GetViewportDimensions().x, (int)GetViewportDimensions().y };
	myBloomRenderTargets[0] = RenderTarget::Create(viewPort, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	for (int i = 1; i < myBloomRenderTargets.size(); i++)
	{
		myBloomRenderTargets[i] = RenderTarget::Create(viewPort / scalar, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
		scalar *= 2;
	}
	myComicShaderRenderTarget.Clear(myClearColor);
	myComicShaderRenderTarget = RenderTarget::Create({ (int)GetViewportDimensions().x,(int)GetViewportDimensions().y }, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	return true;
}

void GraphicsEngine::UpdateFrameBuffer(bool aUseDepth, BlendState aBlendState)
{
	//TODO: fix ugly stuff dont do this c: //Yes yes I fix
	FrameBufferData frameBufferData = {};
	frameBufferData.resolution = { (float)myWindowSize.x, (float)myWindowSize.y, myViewport.x, myViewport.y };
	frameBufferData.worldToClipMatrix = myCamera->GetModelMatrix();
	frameBufferData.cameraToProjection = myCamera->GetProjectionMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	myContext->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	memcpy(mappedBuffer.pData, &frameBufferData, sizeof(FrameBufferData));

	myContext->Unmap(myFrameBuffer.Get(), 0);

	myContext->VSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());
	myContext->PSSetConstantBuffers(0, 1, myFrameBuffer.GetAddressOf());

	if (aUseDepth)
	{
		BackBufferptr->SetAsActiveTarget(DepthBufferptr);
	}
	else
	{
		BackBufferptr->SetAsActiveTarget();
	}
	//myContext->OMSetRenderTargets(1, myBackBuffer.GetAddressOf(), aUseDepth ? myDepthBuffer.Get() : nullptr);

	SetBlendState(aBlendState);
	myContext->OMSetBlendState(myBlendStates[(int)myRenderstate.myBlendState].Get(), nullptr, 0xffffffff);
}

bool GraphicsEngine::CreateBlendStates()
{
	HRESULT result = S_OK;
	D3D11_BLEND_DESC blendStateDescription = {};

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::Disabled].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AlphaBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AdditiveBlend].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	blendStateDescription = {};
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;
	result = myDevice->CreateBlendState(&blendStateDescription, myBlendStates[(int)BlendState::AddAndFadeBackground].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}


	return true;
}

bool GraphicsEngine::CreateDepthStencilStates()
{
	HRESULT result = S_OK;

	D3D11_DEPTH_STENCIL_DESC readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepthDesc.StencilEnable = false;

	result = myDevice->CreateDepthStencilState(&readOnlyDepthDesc, &myDepthStencilStates[(int)DepthStencilState::ReadOnly]);
	if (FAILED(result))
		return false;

	readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	readOnlyDepthDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepthDesc,
		&myDepthStencilStates[(int)DepthStencilState::ReadOnlyGreater]);
	if (FAILED(result))
		return false;

	readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	readOnlyDepthDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepthDesc,
		&myDepthStencilStates[(int)DepthStencilState::ReadOnlyLess]);
	if (FAILED(result))
		return false;

	readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	readOnlyDepthDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepthDesc,
		&myDepthStencilStates[(int)DepthStencilState::ReadLessEqual]);
	if (FAILED(result))
		return false;

	readOnlyDepthDesc = {};
	readOnlyDepthDesc.DepthEnable = true;
	readOnlyDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	readOnlyDepthDesc.StencilEnable = false;
	result = myDevice->CreateDepthStencilState(&readOnlyDepthDesc,
		&myDepthStencilStates[(int)DepthStencilState::ReadGreaterEqual]);
	if (FAILED(result))
		return false;


	myDepthStencilStates[(int)DepthStencilState::Write] = nullptr;

	return true;
}

bool GraphicsEngine::CreateRasterizerStates()
{
	HRESULT result = S_OK;

	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = true;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::Wireframe]);
	if (FAILED(result))
		return false;

	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::WireframeNoCulling]);
	if (FAILED(result))
		return false;

	rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::NoFaceCulling]);
	if (FAILED(result))
		return false;

	rasterizerDesc = {};
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::FrontfaceCulling]);

	result = myDevice->CreateRasterizerState(&rasterizerDesc, &myRasterizerStates[(int)RasterizerState::FrontCulling]);
	if (FAILED(result))
		return false;

	myRasterizerStates[(int)RasterizerState::BackfaceCulling] = nullptr;

	return true;
}

bool GraphicsEngine::CreateSamplers()
{
	HRESULT result = S_OK;
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Point][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Bilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Clamp].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Wrap].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = myDevice->CreateSamplerState(&samplerDesc, mySamplerStates[(int)SamplerFilter::Trilinear][(int)SamplerAddressMode::Mirror].ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateBuffer(uint32_t aSize, ComPtr<ID3D11Buffer>& aBuffer)
{
	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = aSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	result = myDevice->CreateBuffer(&bufferDesc, NULL, aBuffer.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool GraphicsEngine::CreateConstantBuffers()
{
	if (!CreateBuffer(sizeof(ObjectBufferData), myObjectBuffer))
	{
		return false;
	}
	if (!CreateBuffer(sizeof(FrameBufferData), myFrameBuffer))
	{
		return false;
	}
	if (!CreateBuffer(sizeof(LightBufferData), myLightBuffer))
	{
		return false;
	}
	if (!CreateBuffer(sizeof(CameraBufferData), myCameraBuffer))
	{
		return false;
	}
	if (!CreateBuffer(sizeof(PostProcessConstantBufferData), myPostProcessBuffer))
	{
		return false;
	}
	return true;
}

void GraphicsEngine::SavePostprocessSettings(const char* aPath)
{
	std::ifstream file;
	std::string filepath = STATIC_SETTINGS_PATH;
	filepath += aPath;

	nlohmann::json json;
	json["Bloomblending"] = myBloomBlending;
	json["Exposure"] = myExposure;
	json["Saturation"] = mySaturation;
	json["Tint"]["x"] = myTint.x;
	json["Tint"]["y"] = myTint.y;
	json["Tint"]["z"] = myTint.z;
	json["Contrast"]["x"] = myContrast.x;
	json["Contrast"]["y"] = myContrast.y;
	json["Contrast"]["z"] = myContrast.z;
	json["Blackpoint"]["x"] = myBlackpoint.x;
	json["Blackpoint"]["y"] = myBlackpoint.y;
	json["Blackpoint"]["z"] = myBlackpoint.z;

	file.open(filepath.c_str());
	std::ofstream fileOut;
	fileOut.open(filepath);
	if (fileOut.is_open())
	{
		fileOut << json.dump(4);
	}
	fileOut.close();
}

void GraphicsEngine::ReadPostprocessSettings(const int aPostProcessIndex)
{
	if (aPostProcessIndex < 0) return;
	std::ifstream file;
	std::string filepath = STATIC_POSTPROCESSING_PATH;
	filepath += myPostProccessArray[aPostProcessIndex];

	file.open(filepath.c_str());
	if (file.is_open())
	{
		nlohmann::json json;
		file >> json;
		try
		{
			myBloomBlending = json["Bloomblending"];
			myExposure = json["Exposure"];
			mySaturation = json["Saturation"];
			myTint.x = json["Tint"]["x"];
			myTint.y = json["Tint"]["y"];
			myTint.z = json["Tint"]["z"];
			myContrast.x = json["Contrast"]["x"];
			myContrast.y = json["Contrast"]["y"];
			myContrast.z = json["Contrast"]["z"];
			myBlackpoint.x = json["Blackpoint"]["x"];
			myBlackpoint.y = json["Blackpoint"]["y"];
			myBlackpoint.z = json["Blackpoint"]["z"];
		}
		catch (const std::exception&)
		{
			LogError("Failed to load postprocess for " + filepath);
		}
	}
	else
	{
		nlohmann::json json;
		json["Bloomblending"] = myBloomBlending;
		json["Exposure"] = myExposure;
		json["Saturation"] = mySaturation;
		json["Tint"]["x"] = myTint.x;
		json["Tint"]["y"] = myTint.y;
		json["Tint"]["z"] = myTint.z;
		json["Contrast"]["x"] = myContrast.x;
		json["Contrast"]["y"] = myContrast.y;
		json["Contrast"]["z"] = myContrast.z;
		json["Blackpoint"]["x"] = myBlackpoint.x;
		json["Blackpoint"]["y"] = myBlackpoint.y;
		json["Blackpoint"]["z"] = myBlackpoint.z;


		std::ofstream fileOut;
		fileOut.open(filepath);
		if (fileOut.is_open())
		{
			fileOut << json.dump(4);
		}
		fileOut.close();
	}
	file.close();
	myCurrentPostProcessingPath = myPostProccessArray[aPostProcessIndex];
}
