#pragma once
#include "Component.h"
#include "../graphics/GraphicsEngine.h"


class CameraComponent : public Component
{
private:
	DirectX::XMMATRIX myProjectionMatrix;
	DirectX::XMMATRIX myCopyMatrix;
	DirectX::XMMATRIX myTargetMatrix;

	//Temp project 6
	const Transform* myFollowPos;
	Vector3f myDiffPos;

	float myFOV;
	float myNearPlane;
	float myFarPlane;
	float myWidth;
	float myHeight;
	float myHFar;
	float myWFar;
	float myHNear;
	float myWNear;
	float myHFoV;
	float myVFoV;

	bool myShaking; // Flag to track if the camera is currently shaking
	float myShakeFrequency; // Magnitude of the shake effect
	float myShakeDuration; // Duration of the shake effect
	float myShakeTimer; // Timer to track how long the shake has been active
	float myShakeIntensity; // Timer to track how long the shake has been active

	bool myIsCameraEventActive;
	float myLerpDuration;
	float myLerpTimer;

	//TODO: Move this function OUT, souldnt be in CAMERA TF
	DirectX::XMMATRIX LerpMatrix(const DirectX::XMMATRIX& start, const DirectX::XMMATRIX& end, float t)
	{
		return start + t * (end - start);
	}

public:
	CameraComponent();
	~CameraComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();

	void SetPerspectiveProjection(
		const float& aFov,
		const float& aWidth,
		const float& aHeight,
		const float& aNearPlane,
		const float& aFarPlane
	);

	DirectX::XMMATRIX GetProjectionMatrix() const;
	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetModelMatrix() const;

	Vector3f GetForward() const;
	Vector3f GetRight() const;
	Vector3f GetUp() const;

	float GetFOV() const { return myFOV / (3.14159265359f / 180.f); }
	float GetNearPlane() const { return myNearPlane; }
	float GetFarPlane() const { return myFarPlane; }

	const Vector2<float> GetNearPlaneDimensions() const { return { myWNear, myHNear }; }
	const Vector2<float> GetFarPlaneDimensions() const { return { myWFar, myHFar }; }

	void SetOrtographicProjection(float aWidth, float aHeight, float aDepth);
	void SetOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar);

	void Shake(float frequency, float duration, float intensity);

	void StartCameraEvent(const DirectX::XMMATRIX& targetMatrix, float lerpDuration);

	bool IsInFrustum(const DirectX::XMFLOAT3& objectPosition, float objectRadius) const;

	//Temp project 6
	void SetFollowPosition(const Transform* aFollowPos);

	void Reset() override
	{
		//Does not need reset
	}
};