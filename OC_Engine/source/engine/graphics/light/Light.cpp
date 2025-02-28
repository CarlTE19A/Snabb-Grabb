#include "stdafx.h"

#include "Light.h"

Light::Light(const Transform& aTransform, const Vector4<float> aColor, float aIntensity)
	: myTransform(aTransform)
	, myColor(aColor)
	, myIntensity(aIntensity)
{}
