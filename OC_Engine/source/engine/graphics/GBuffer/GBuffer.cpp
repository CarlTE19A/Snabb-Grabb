#include "stdafx.h"
#include "GBuffer.h"
#include <../engine/graphics/DepthBuffer.h>
#include "../GraphicsEngine.h"
#include <Engine.h>

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
}

GBuffer GBuffer::Create(Vector2<int> aSize)
{
	auto& device = GraphicsEngine::GetInstance()->GetDevice();
	HRESULT result;
	std::array<DXGI_FORMAT, (int)GBufferTexture::Count> textureFormats =
	{
	DXGI_FORMAT_R32G32B32A32_FLOAT,// Position
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,// Albedo
	DXGI_FORMAT_R10G10B10A2_UNORM,// Normal,
	DXGI_FORMAT_R8G8B8A8_UNORM,// Material
	DXGI_FORMAT_R8G8B8A8_UNORM// EffectsAndCustom
	};

	GBuffer returnGBuffer;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	for (unsigned int idx = 0; idx < (int)GBufferTexture::Count; idx++)
	{
		desc.Format = textureFormats[idx];
		result = device->CreateTexture2D(&desc, nullptr, &returnGBuffer.myTextures[idx]);
		assert(SUCCEEDED(result));
		result = device->CreateRenderTargetView(
			returnGBuffer.myTextures[idx].Get(),
			nullptr,
			returnGBuffer.myRTVs[idx].GetAddressOf());
		assert(SUCCEEDED(result));
		result = device->CreateShaderResourceView(
			returnGBuffer.myTextures[idx].Get(),
			nullptr,
			returnGBuffer.mySRVs[idx].GetAddressOf());
		assert(SUCCEEDED(result));
	}
	returnGBuffer.myViewport = std::make_shared<D3D11_VIEWPORT>(
		D3D11_VIEWPORT{
		0,
		0,
		static_cast<float>(desc.Width),
		static_cast<float>(desc.Height),
		0,
		1
		});
	return returnGBuffer;
}

void GBuffer::ClearTextures()
{
	GraphicsEngine& graphicsEngine = *GraphicsEngine::GetInstance();
	auto context = graphicsEngine.GetContext();
	for (unsigned int idx = 0; idx < (int)GBufferTexture::Count; idx++)
	{
		context->ClearRenderTargetView(myRTVs[idx].Get(), graphicsEngine.GetClearColor());
	}
}

void GBuffer::SetAsActiveTarget(DepthBuffer* aDepthBuffer)
{
	auto context = GraphicsEngine::GetInstance()->GetContext();
	if (aDepthBuffer)
	{
		context->OMSetRenderTargets((int)GBufferTexture::Count, myRTVs[0].GetAddressOf(), aDepthBuffer->GetDepthStencilView());
	}
	else
	{
		context->OMSetRenderTargets((int)GBufferTexture::Count, myRTVs[0].GetAddressOf(), nullptr);
	}
	context->RSSetViewports(1, myViewport.get());
}

void GBuffer::SetAsResourceOnSlot(GBufferTexture aTexture, unsigned int aSlot)
{
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(aSlot, 1, mySRVs[(int)aTexture].GetAddressOf());
	GraphicsEngine::GetInstance()->GetContext()->CSSetShaderResources(aSlot, 1, mySRVs[(int)aTexture].GetAddressOf());
}

void GBuffer::SetAllAsResources(unsigned aSlot)
{
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(aSlot, (int)GBufferTexture::Count, mySRVs[0].GetAddressOf());
}

void GBuffer::ClearAllAsResources(unsigned int aSlot)
{
	ID3D11ShaderResourceView* const nullSRV[6] = { NULL };
	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(aSlot, (int)GBufferTexture::Count, nullSRV);
}
