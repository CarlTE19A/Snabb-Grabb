#pragma once
#include "math/Transform.h"

class Light
{
protected:
	Vector4f myColor;
	Transform myTransform;
	float myIntensity;


public:
	virtual ~Light() = default;

	Light(const Transform& aTransform, const Vector4<float> aColor, float aIntensity);

	inline float GetIntensity() const { return myIntensity; }
	inline Vector4f GetColor() const { return myColor; }
	inline const Transform& GetTransform() const { return myTransform; }

	inline void SetTransform(Transform aTransform) { myTransform = aTransform; }
	inline void SetPosition(Vector3f aPosition) { myTransform.SetPosition(aPosition); }
	inline void SetRotation(Vector3f aRotation) { myTransform.SetRotation(aRotation); }
	inline void SetColor(Vector4f aColor) { myColor = aColor; }
	inline void SetIntensity(float aIntensity) { myIntensity = aIntensity; }
};