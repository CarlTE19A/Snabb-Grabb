#pragma once
#include "Light.h"

class SpotLight : public Light
{
public:
	SpotLight() : Light(Transform{}, {}, {}), myOuterLimit(0), myInnerLimit(0), myRange(0), myViewPortIndex(-1) {};
	SpotLight(const Transform& someTransform, const Vector4f& aColor, float aIntensity, float aOuterAngle, float aInnerAngle, float aRange);
	virtual ~SpotLight() = default;

	inline float GetRange() const { return myRange; }
	inline float GetOuterLimit() const { return myOuterLimit; }
	inline float GetInnerLimit() const { return myInnerLimit; }
	inline void SetInnerLimit(float aInnerLimit) { myInnerLimit = /*cos*/(aInnerLimit); }
	inline void SetOuterLimit(float aOuterLimit) { myOuterLimit = /*cos*/(aOuterLimit); }
	inline void SetRange(float aRange) { myRange = aRange; }

	inline void SetLightProjectionMatrix(const DirectX::XMMATRIX aProjectionMatrix) { myProjectionMatrix = aProjectionMatrix; }
	inline const DirectX::XMMATRIX& GetLightProjectionMatrix() { return myProjectionMatrix; }

	int myViewPortIndex;
	Vector2f myViewPortSize;
	Vector2f myTopLeftXY;
private:
	float myOuterLimit;
	float myInnerLimit;
	float myRange;

	DirectX::XMMATRIX myProjectionMatrix;
};