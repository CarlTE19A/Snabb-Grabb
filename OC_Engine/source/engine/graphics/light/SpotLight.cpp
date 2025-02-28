#include "stdafx.h"

#include "SpotLight.h"

SpotLight::SpotLight(const Transform& someTransform, const Vector4f& aColor, float aIntensity, float aOuterAngle, float aInnerAngle, float aRange)
	: Light(someTransform, aColor, aIntensity)
{
	myInnerLimit = cos(aInnerAngle);
	myOuterLimit = cos(aOuterAngle);
	myRange = aRange;
	myViewPortIndex = -1;
}
