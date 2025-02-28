#pragma once
#include <wrl/client.h>
#include "Light.h"
//#include <string>

using Microsoft::WRL::ComPtr;
struct ID3D11ShaderResourceView;

class AmbientLight : public Light
{
public:
	AmbientLight();
	AmbientLight(const Vector4f& aColor, float aIntensity);
	AmbientLight(std::wstring& someCubemapLocation, const Vector4f& aColor, float aIntensity);
	AmbientLight(const AmbientLight&);
	AmbientLight& operator=(const AmbientLight& other);

	virtual ~AmbientLight();

	void SetCubemap(std::wstring someCubemapLocation);

	inline ID3D11ShaderResourceView* const* GetCubemap() const { return myCubemap.GetAddressOf(); }
	inline unsigned int GetNumMips() const { return myNumMips; }

private:
	ComPtr<ID3D11ShaderResourceView> myCubemap = nullptr;
	unsigned int myNumMips = 0;
};