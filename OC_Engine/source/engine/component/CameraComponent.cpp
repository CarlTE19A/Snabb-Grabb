#include "stdafx.h"

#include "CameraComponent.h"
#include "../gameObject/GameObject.h"

CameraComponent::CameraComponent()
{
	myProjectionMatrix = DirectX::XMMatrixIdentity();
	myCopyMatrix = DirectX::XMMatrixIdentity();
	myTargetMatrix = DirectX::XMMatrixIdentity();
	myFollowPos = nullptr;
	myDiffPos = {};
	myFOV = 0;
	myNearPlane = 0;
	myFarPlane = 0;
	myHFar = 0;
	myWFar = 0;
	myHNear = 0;
	myWNear = 0;
	myHFoV = 0;
	myVFoV = 0;
	myHeight = 0;
	myWidth = 0;
	myShaking = false;
	myShakeFrequency = 20.0f;
	myShakeDuration = 0.0f;
	myShakeTimer = 0.0f;
	myShakeIntensity = 1.0f;
	myIsCameraEventActive = false;
	myLerpDuration = 0.0f;
	myLerpTimer = 0.0f;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update(const float aDeltaTime, Scene& aScene)
{
	if (!IsActive()) return;
	aDeltaTime;
	aScene;

	//Temp for project 6
	if (myFollowPos != nullptr)
	{
		myParent->GetTransform()->SetPosition(Vector3f(
			myFollowPos->GetPosition().x + myDiffPos.x,
			myFollowPos->GetPosition().y + myDiffPos.y,
			myFollowPos->GetPosition().z + myDiffPos.z));
	}

	if (myShaking)
	{
		myShakeTimer += aDeltaTime;

		if (myShakeTimer >= myShakeDuration)
		{
			myShaking = false;
			myShakeTimer = 0.0f;
			//myParent->GetTransform()->SetMatrix(myCopyMatrix);
		}
		else
		{
			// Generate a random offset for the shake effect
			//more rounded smoother shake
			float smoothOffsetX = cos(myShakeTimer * myShakeFrequency) * (myShakeIntensity + ((rand() % 100) / 100.0f) * 0.2f);
			float smoothOffsetY = sin(myShakeTimer * myShakeFrequency) * (myShakeIntensity + ((rand() % 100) / 100.0f) * 0.2f);
			//less rounded more jagged shake
			//float offsetX = (rand() % 100) / 100.0f * myShakeFrequency - myShakeFrequency / 2.0f;
			//float offsetY = (rand() % 100) / 100.0f * myShakeFrequency - myShakeFrequency / 2.0f;

			// Create a translation matrix for the shake effect
			DirectX::XMMATRIX shakeMatrix = DirectX::XMMatrixTranslation(smoothOffsetX, smoothOffsetY, 0.0f);

			// Apply the shake effect to the view matrix
			//myParent->GetTransform()->SetMatrix(myCopyMatrix * shakeMatrix);
			myParent->GetTransform()->SetMatrix(myParent->GetTransform()->GetMatrix() * shakeMatrix);
		}
		//myCopyMatrix = myParent->GetTransform()->GetMatrix();
		
	}
	if (myIsCameraEventActive)
	{
		myLerpTimer += aDeltaTime;

		if (myLerpTimer >= myLerpDuration)
		{
			myIsCameraEventActive = false;
			myLerpTimer = 0.0f;
			myParent->GetTransform()->SetMatrix(myTargetMatrix);
		}
		else
		{
			float t = myLerpTimer / myLerpDuration;
			DirectX::XMMATRIX lerpedMatrix = LerpMatrix(myParent->GetTransform()->GetMatrix(), myTargetMatrix, t);
			myParent->GetTransform()->SetMatrix(lerpedMatrix);
		}
	}

}

void CameraComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Camera"))
	{
		ImguiNativeUpdate();
		if (ImGui::BeginItemTooltip())
		{
			ImGui::Text("Disable this will NOT disable the camera itself, only the component.");
			ImGui::EndTooltip();
		}

		float* newFoV = new float(GetFOV());
		float oldFoV = *newFoV;
		ImGui::DragFloat("FOV", newFoV, 0.01f, 0, 179.9f);

		float* newNearPlane = new float(myNearPlane);
		ImGui::DragFloat("Near Plane", newNearPlane, 1.f, 0.01f, myFarPlane);

		float* newFarPlane = new float(myFarPlane);
		ImGui::DragFloat("Far Plane", newFarPlane, 1.f, *newNearPlane, 100000);

		if (*newFoV != oldFoV || *newNearPlane != myNearPlane || *newFarPlane != myFarPlane)
		{
			SetPerspectiveProjection(*newFoV, myWidth, myHeight, *newNearPlane, *newFarPlane);
			//SetProjectionMatrix(90.f, myWidth, myHeight, 0.1f, 100000.f);
		}

		if (ImGui::Button("Render Camera"))
		{
			GraphicsEngine::GetInstance()->SetCamera(this);
		}

		//float* shakeFrequency = new float(myShakeFrequency);
		//ImGui::DragFloat("Shake Frequency", shakeFrequency, 0.1f, 0, 100);
		//float* shakeDuration = new float(myShakeDuration);
		//ImGui::DragFloat("Shake Duration", shakeDuration, 0.1f, 0, 10);
		//float* shakeIntensity = new float(myShakeIntensity);
		//ImGui::DragFloat("Shake Intensity", shakeIntensity, .01f, 0, 10);
		//
		//myShakeDuration = *shakeDuration;
		//myShakeFrequency = *shakeFrequency;
		//myShakeIntensity = *shakeIntensity;
		//
		//if (ImGui::Button("Shake")) 
		//{
		//	Shake(*shakeFrequency, *shakeDuration, *shakeIntensity);
		//}
		//
		//delete shakeFrequency;
		//delete shakeDuration;
		//delete shakeIntensity;
		delete newFoV;
		delete newNearPlane;
		delete newFarPlane;
		ImGui::EndTabItem();
	};
}

void CameraComponent::SetPerspectiveProjection(
	const float& aFov,
	const float& aWidth,
	const float& aHeight,
	const float& aNearPlane,
	const float& aFarPlane
)
{
	myFOV = aFov * (3.14159265359f / 180.f);
	myNearPlane = aNearPlane;
	myFarPlane = aFarPlane;
	myWidth = aWidth;
	myHeight = aHeight;

	myHFoV = aFov * 3.14159265359f / 180.f;
	myVFoV = 2 * std::atan(std::tan(myHFoV / 2) * (aHeight / aWidth));

	myHFar = 2 * tan(myHFoV / 2.f) * aFarPlane;
	myWFar = myHFar * myHFoV / myVFoV;
	myHNear = 2 * tan(myHFoV / 2.f) * aNearPlane;
	myWNear = myHNear * myHFoV / myVFoV;
	float Q = myFarPlane / (myFarPlane - myNearPlane);

	myProjectionMatrix = DirectX::XMMatrixIdentity();

	myProjectionMatrix.r[0].m128_f32[0] = 1 / tan(myHFoV / 2);
	myProjectionMatrix.r[1].m128_f32[1] = (1 / tan(myVFoV / 2));
	myProjectionMatrix.r[2].m128_f32[2] = Q;
	myProjectionMatrix.r[2].m128_f32[3] = 1.f / Q;
	myProjectionMatrix.r[3].m128_f32[2] = -Q * myNearPlane;
	myProjectionMatrix.r[3].m128_f32[3] = 0;
}

void CameraComponent::SetOrtographicProjection(float aWidth, float aHeight, float aDepth)
{
	myProjectionMatrix = {};

	myProjectionMatrix.r[0].m128_f32[0] = 2.f / aWidth;
	myProjectionMatrix.r[1].m128_f32[1] = 2.f / aHeight;
	myProjectionMatrix.r[2].m128_f32[2] = 1.f / aDepth;
}

void CameraComponent::SetOrtographicProjection(float aLeft, float aRight, float aTop, float aBottom, float aNear, float aFar)
{
	myProjectionMatrix = DirectX::XMMatrixIdentity();

	myProjectionMatrix.r[0].m128_f32[0] = 2.f / (aRight - aLeft);
	myProjectionMatrix.r[1].m128_f32[1] = 2.f / (aBottom - aTop);
	myProjectionMatrix.r[2].m128_f32[2] = 1.f / (aFar - aNear);

	myProjectionMatrix.r[3].m128_f32[0] = -(aRight + aLeft) / (aRight - aLeft);
	myProjectionMatrix.r[3].m128_f32[1] = -(aBottom + aTop) / (aBottom - aTop);
	myProjectionMatrix.r[3].m128_f32[2] = -(aNear) / (aFar - aNear);
}

void CameraComponent::Shake(float frequency, float duration, float intensity)
{
	myShaking = true;
	myShakeFrequency = frequency;
	myShakeDuration = duration;
	myShakeIntensity = intensity;
	myShakeTimer = 0.0f;
	myCopyMatrix = myParent->GetTransform()->GetMatrix();
}

void CameraComponent::StartCameraEvent(const DirectX::XMMATRIX& targetMatrix, float lerpDuration)
{
	myIsCameraEventActive = true;
	myTargetMatrix = targetMatrix;
	myLerpDuration = lerpDuration;
	myLerpTimer = 0.0f;
}

bool CameraComponent::IsInFrustum(const DirectX::XMFLOAT3& objectPosition, float objectRadius) const
{
	// Get the combined view-projection matrix
	DirectX::XMMATRIX viewProjection = DirectX::XMMatrixInverse(nullptr, GetViewMatrix()) * GetProjectionMatrix();

	// Extract the frustum planes from the view-projection matrix
	DirectX::XMVECTOR planes[6];
	for (int i = 0; i < 6; ++i)
	{
		planes[i] = DirectX::XMPlaneNormalize(DirectX::XMVectorSet(
			viewProjection.r[0].m128_f32[i < 4 ? i : i - 4],
			viewProjection.r[1].m128_f32[i < 4 ? i : i - 4],
			viewProjection.r[2].m128_f32[i < 4 ? i : i - 4],
			viewProjection.r[3].m128_f32[i < 4 ? i : i - 4]
		));
	}

	// Check if the object is inside the frustum
	for (int i = 0; i < 6; ++i)
	{
		DirectX::XMVECTOR objectPos = DirectX::XMLoadFloat3(&objectPosition);

		DirectX::XMVECTOR planeNormal = DirectX::XMVectorSet(
			planes[i].m128_f32[0],
			planes[i].m128_f32[1],
			planes[i].m128_f32[2],
			0.0f
		);

		float d = planes[i].m128_f32[3];

		// Test against the near and far planes
		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(planeNormal, objectPos)) + d + objectRadius < 0.0f)
		{
			return false; // Object is completely outside this frustum plane
		}
	}

	return true; // Object is inside the frustum
}


void CameraComponent::SetFollowPosition(const Transform* aFollowPos)
{

	myDiffPos = myParent->GetTransform()->GetPosition();
	myDiffPos.x -= aFollowPos->GetPosition().x;
	myDiffPos.y -= aFollowPos->GetPosition().y;
	myDiffPos.z -= aFollowPos->GetPosition().z;
	myFollowPos = aFollowPos;
}

DirectX::XMMATRIX CameraComponent::GetProjectionMatrix() const { return myProjectionMatrix; }
DirectX::XMMATRIX CameraComponent::GetViewMatrix() const { return myParent->GetTransform()->GetMatrix(); }
DirectX::XMMATRIX CameraComponent::GetModelMatrix() const { return XMMatrixInverse(nullptr, myParent->GetTransform()->GetMatrix()); }

Vector3f CameraComponent::GetForward() const
{
	return myParent->GetTransform()->GetForward();
}

Vector3f CameraComponent::GetRight() const
{
	return myParent->GetTransform()->GetRight();
}

Vector3f CameraComponent::GetUp() const
{
	return myParent->GetTransform()->GetUp();
}