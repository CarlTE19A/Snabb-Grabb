#include "stdafx.h"

#include "PointLight.h"

PointLight::PointLight(const Transform& someTransform, const Vector4f& aColor, float aIntensity, float someRange)
	: Light(someTransform, aColor, aIntensity), myRange(someRange) {}


void PointLight::SetRange(float someRange)
{
	myRange = someRange;
}
