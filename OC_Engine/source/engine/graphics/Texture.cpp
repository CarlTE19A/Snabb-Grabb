#include "stdafx.h"

#include "GraphicsEngine.h"
#include "stb_image.h"
#include "DDSTextureLoader11.h"

Texture::Texture()
{
	myWidth = 0;
	myHeight = 0;
	myShaderResourceView = nullptr;
	myTexture = nullptr;
}

Texture::Texture(ID3D11ShaderResourceView* aSRV) : myShaderResourceView(aSRV)
{
	myTexture = nullptr;
	myWidth = 0;
	myHeight = 0;
}

Texture::~Texture()
{
	delete myrgbaPixels;
	myTexture = nullptr;
	//if (myTexture != nullptr)
	//{
	//	myTexture->Release();
	//}
};

bool Texture::Initialize(const TextureInitializationType& aType, std::string aPath, const bool aIsUI/*, bool aDoRelative*/)
{
	std::string path = aPath;

	myHasInitialized = true;
	switch (aType)
	{
	case TextureInitializationType::PNG:
		return InitializePNG(path);
	case TextureInitializationType::DDS:
		return InitializeDDS(StringHelper::s2ws(path), aIsUI);
	default:
		break;
	}
	return false;
}

void Texture::Bind(const int slot, ID3D11DeviceContext* context)
{
	if (!myHasInitialized) return;

	context->PSSetShaderResources(slot, 1, myShaderResourceView.GetAddressOf());

	assert("");
}

void Texture::SetShaderResourceView(ID3D11ShaderResourceView* aSRV)
{
	myShaderResourceView = aSRV;
}

void Texture::SetTexture(ID3D11Texture2D& aTexture)
{
	*myTexture = aTexture;
	//myHeight = 200;
	//myWidth = 200;
	D3D11_TEXTURE2D_DESC textureDesc;
	myShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&myTexture));
	myTexture->GetDesc(&textureDesc);

	// Calculate aspect ratio
	myWidth = textureDesc.Width;
	myHeight = textureDesc.Height;
	// Now, you can use 'aspectRatio' as needed
	myHasInitialized = true;
}

ComPtr<ID3D11ShaderResourceView> Texture::GetVeiw()
{
	return myShaderResourceView;
}

bool Texture::InitializePNG(const std::string& aPath)
{
	myPath = aPath;
	int width, height, channels;
	bool useSrgb = true; // temp?

	unsigned char* rgbaPixels = stbi_load((std::string(RELATIVE_ASSET_PATH) + aPath).c_str(), &width, &height, &channels, 0);
	if (rgbaPixels == nullptr)
		return false;

	if (channels < 3 || channels > 4)
		return false;

	if (channels == 3)
	{
		std::vector<unsigned char> imageData(width * height * 4);
		for (int i = 0; i < width * height; i++)
		{
			imageData[4 * i + 0] = rgbaPixels[3 * i + 0];
			imageData[4 * i + 1] = rgbaPixels[3 * i + 1];
			imageData[4 * i + 2] = rgbaPixels[3 * i + 2];
			imageData[4 * i + 3] = 255;
		}
		rgbaPixels = imageData.data();
	}

	myHeight = height;
	myWidth = width;

	myrgbaPixels = rgbaPixels;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	if (useSrgb)
		desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	else
		desc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	auto device = GraphicsEngine::GetInstance()->GetDevice();
	HRESULT hra = device->CreateTexture2D(&desc, nullptr, &myTexture);
	if (FAILED(hra))
	{
		return false;
	}

	//update subresources
	GraphicsEngine::GetInstance()->GetContext()->UpdateSubresource(myTexture, 0, nullptr, (void*)myrgbaPixels, myWidth * 4, myWidth * myHeight * 4);

	HRESULT hr = device->CreateShaderResourceView(myTexture, NULL, &myShaderResourceView);
	if (FAILED(hr))
	{
		return false;
	}

	GraphicsEngine::GetInstance()->GetContext()->GenerateMips(myShaderResourceView.Get());
	myTexture->Release();

	return true;
}

bool Texture::InitializeDDS(const std::wstring& aPath, const bool aIsUI)
{
	myPath = StringHelper::ws2s(aPath);
	std::transform(myPath.begin(), myPath.end(), myPath.begin(),
		[](unsigned char c) { return std::tolower(c); });
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	if(!aIsUI)
	{
		result = DirectX::CreateDDSTextureFromFileEx(
			device.Get(),
			aPath.c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			DirectX::DDS_LOADER_DEFAULT,
			nullptr,
			&myShaderResourceView
		);
	}
	else
	{
		result = DirectX::CreateDDSTextureFromFileEx(
			device.Get(),
			aPath.c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			DirectX::DDS_LOADER_FORCE_SRGB,
			nullptr,
			&myShaderResourceView);
	}
		
	if (!FAILED(result))
	{
		// Successfully loaded the texture, now calculate aspect ratio
		D3D11_TEXTURE2D_DESC textureDesc;
		myShaderResourceView->GetResource(reinterpret_cast<ID3D11Resource**>(&myTexture));
		myTexture->GetDesc(&textureDesc);

		// Calculate aspect ratio
		myWidth = textureDesc.Width;
		myHeight = textureDesc.Height;
		// Now, you can use 'aspectRatio' as needed
	}

	return !FAILED(result);
}
