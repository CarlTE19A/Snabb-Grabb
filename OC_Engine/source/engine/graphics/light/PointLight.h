#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight() : Light(Transform{}, {}, {}) { myRange = 0.f; };
	PointLight(const Transform& someTransform, const Vector4f& aColor, float aIntensity, float someRange);
	virtual ~PointLight() = default;

	void SetRange(float someRange);
	inline float GetRange() const { return myRange; }

private:
	float myRange;
};