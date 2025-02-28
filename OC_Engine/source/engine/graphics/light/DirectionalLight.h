#pragma once
#include "Light.h"
#include <DirectXMath.h>

class DirectionalLight : public Light
{
public:

	DirectionalLight() : Light(Transform{}, {}, {}), myProjectionMatrix(DirectX::XMMatrixIdentity()), myViewPortIndex(-1){};
	DirectionalLight(const Transform& aTransform, const Vector4f& aColor, float aIntensity) : Light(aTransform, aColor, aIntensity), myProjectionMatrix(DirectX::XMMatrixIdentity()) {}
	virtual ~DirectionalLight() = default;
	inline void SetLightProjectionMatrix(const DirectX::XMMATRIX aProjectionMatrix) { myProjectionMatrix = aProjectionMatrix; }
	inline const DirectX::XMMATRIX& GetLightProjectionMatrix() { return myProjectionMatrix; }

	int myViewPortIndex;
	Vector2f myViewPortSize;
	Vector2f myTopLeftXY;
private:
	DirectX::XMMATRIX myProjectionMatrix;
};