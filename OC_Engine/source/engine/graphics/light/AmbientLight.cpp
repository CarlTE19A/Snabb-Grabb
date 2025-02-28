#include "stdafx.h"

#include "AmbientLight.h"

#include <cassert>
#include "../DDSTextureLoader11.h"
#include "../GraphicsEngine.h"

AmbientLight::AmbientLight() : Light(Transform{}, {}, {}) {}
AmbientLight::AmbientLight(const Vector4f& aColor, float aIntensity) : Light(Transform{}, aColor, aIntensity) {}
AmbientLight::AmbientLight(std::wstring& someCubemapLocation, const Vector4f& aColor, float aIntensity) : Light(Transform{}, aColor, aIntensity) { SetCubemap(someCubemapLocation); }
AmbientLight::AmbientLight(const AmbientLight&) = default;
AmbientLight::~AmbientLight() {}
AmbientLight& AmbientLight::operator=(const AmbientLight& other) = default;

void AmbientLight::SetCubemap(std::wstring someCubemapLocation)
{
	auto& device = GraphicsEngine::GetInstance()->GetDevice();
	ID3D11Resource* theResource;
	HRESULT result = DirectX::CreateDDSTextureFromFile(device.Get(), someCubemapLocation.c_str(), &theResource, &myCubemap);
	assert(!FAILED(result));
	result;
	//myCubemap->GetResource(&theResource);
	ID3D11Texture2D* cubeTexture = reinterpret_cast<ID3D11Texture2D*>(theResource);
	D3D11_TEXTURE2D_DESC cubeDescription;
	cubeTexture->GetDesc(&cubeDescription);

	myNumMips = cubeDescription.MipLevels;
}
