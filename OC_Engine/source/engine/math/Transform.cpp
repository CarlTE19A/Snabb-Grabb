#include "Transform.h"
#include "Transform.h"
#include "stdafx.h"

#include "Transform.h"
#include <iostream>

//define function that creates a dx11 vector from a Vector3f
DirectX::XMVECTOR CreateDXVector(const Vector3f& aVector)
{
	return DirectX::XMVectorSet(aVector.x, aVector.y, aVector.z, 1.0f);
}

void Transform::ResetRotationAndScale()
{
	myMatrix.r[0].m128_f32[0] = 1.0f;
	myMatrix.r[0].m128_f32[1] = 0.0f;
	myMatrix.r[0].m128_f32[2] = 0.0f;
	myMatrix.r[0].m128_f32[3] = 0.0f;
	myMatrix.r[1].m128_f32[0] = 0.0f;
	myMatrix.r[1].m128_f32[1] = 1.0f;
	myMatrix.r[1].m128_f32[2] = 0.0f;
	myMatrix.r[1].m128_f32[3] = 0.0f;
	myMatrix.r[2].m128_f32[0] = 0.0f;
	myMatrix.r[2].m128_f32[1] = 0.0f;
	myMatrix.r[2].m128_f32[2] = 1.0f;
	myMatrix.r[2].m128_f32[3] = 0.0f;
}

Transform::Transform()
{
	myMatrix = DirectX::XMMatrixIdentity();
}

void Transform::ResetMatrix()
{
	myMatrix = DirectX::XMMatrixIdentity();
}

void Transform::SetMatrixFromPxMat44(const physx::PxMat44& aMatrix)
{
	myMatrix.r[0].m128_f32[0] = aMatrix.column0.x;
	myMatrix.r[0].m128_f32[1] = aMatrix.column0.y;
	myMatrix.r[0].m128_f32[2] = aMatrix.column0.z;
	myMatrix.r[0].m128_f32[3] = aMatrix.column0.w;
	myMatrix.r[1].m128_f32[0] = aMatrix.column1.x;
	myMatrix.r[1].m128_f32[1] = aMatrix.column1.y;
	myMatrix.r[1].m128_f32[2] = aMatrix.column1.z;
	myMatrix.r[1].m128_f32[3] = aMatrix.column1.w;
	myMatrix.r[2].m128_f32[0] = aMatrix.column2.x;
	myMatrix.r[2].m128_f32[1] = aMatrix.column2.y;
	myMatrix.r[2].m128_f32[2] = aMatrix.column2.z;
	myMatrix.r[2].m128_f32[3] = aMatrix.column2.w;
	myMatrix.r[3].m128_f32[0] = aMatrix.column3.x;
	myMatrix.r[3].m128_f32[1] = aMatrix.column3.y;
	myMatrix.r[3].m128_f32[2] = aMatrix.column3.z;
	myMatrix.r[3].m128_f32[3] = aMatrix.column3.w;

}

physx::PxMat44 Transform::GetPxMat44()
{
	physx::PxMat44 matrix;
	matrix.column0.x = myMatrix.r[0].m128_f32[0];
	matrix.column0.y = myMatrix.r[0].m128_f32[1];
	matrix.column0.z = myMatrix.r[0].m128_f32[2];
	matrix.column0.w = myMatrix.r[0].m128_f32[3];
	matrix.column1.x = myMatrix.r[1].m128_f32[0];
	matrix.column1.y = myMatrix.r[1].m128_f32[1];
	matrix.column1.z = myMatrix.r[1].m128_f32[2];
	matrix.column1.w = myMatrix.r[1].m128_f32[3];
	matrix.column2.x = myMatrix.r[2].m128_f32[0];
	matrix.column2.y = myMatrix.r[2].m128_f32[1];
	matrix.column2.z = myMatrix.r[2].m128_f32[2];
	matrix.column2.w = myMatrix.r[2].m128_f32[3];
	matrix.column3.x = myMatrix.r[3].m128_f32[0];
	matrix.column3.y = myMatrix.r[3].m128_f32[1];
	matrix.column3.z = myMatrix.r[3].m128_f32[2];
	matrix.column3.w = myMatrix.r[3].m128_f32[3];
	//physx::PxTransform transform(matrix);

	return matrix;
}

void Transform::SetPosition(const Vector3f& aPosition)
{
	myMatrix.r[3] = DirectX::XMVectorSet(aPosition.x, aPosition.y, aPosition.z, 1.0f);
}

void Transform::SetRotation(const Vector3f& aRotation)
{
	//std::cout << "SetRotation is DEPRECATED - Do not directly set rotations, use RotateAroundX/Y/Z or RotateAroundAxis" << std::endl;
	//Vector3f oldScale;
	//Quaternionf oldRot;
	//Vector3f oldPos;
	//
	//DecomposeMatrix(oldScale, oldRot, oldPos);
	//
	//Vector3f rot = DegreesToRadians(aRotation);
	//float	a = cos(rot.x), b = sin(rot.x),
	//		c = cos(rot.y), d = sin(rot.y),
	//		e = cos(rot.z), f = sin(rot.z);
	//
	//float ae = a * e, af = a * f, be = b * e, bf = b * f;
	//
	//myMatrix.r[0].m128_f32[0] = c * e;
	//myMatrix.r[1].m128_f32[0] = -c * f;
	//myMatrix.r[2].m128_f32[0] = d;
	//
	//myMatrix.r[0].m128_f32[1] = af + be * d;
	//myMatrix.r[1].m128_f32[1] = ae - bf * d;
	//myMatrix.r[2].m128_f32[1] = -b * c;
	//
	//myMatrix.r[0].m128_f32[2] = bf - ae * d;
	//myMatrix.r[1].m128_f32[2] = be + af * d;
	//myMatrix.r[2].m128_f32[2] = a * c;
	//
	//myMatrix.r[0].m128_f32[3] = 0.0f;
	//myMatrix.r[1].m128_f32[3] = 0.0f;
	//myMatrix.r[2].m128_f32[3] = 0.0f;

	

	ResetRotationAndScale();
	//myMatrix *= 
	Vector3f radianRotation = DegreesToRadians(aRotation);
	DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationRollPitchYaw(radianRotation.x, radianRotation.y, radianRotation.z);

	myMatrix *= DirectX::XMMatrixRotationQuaternion(quaternion);

	//myMatrix *= DirectX::XMMatrixScalingFromVector(CreateDXVector(oldScale));
}

void Transform::SetScale(const Vector3f& aScale)
{
	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMVECTOR translationVector;
	DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, myMatrix);

	scaleVector.m128_f32[0] = aScale.x;
	scaleVector.m128_f32[1] = aScale.y;
	scaleVector.m128_f32[2] = aScale.z;

	myMatrix = DirectX::XMMatrixScalingFromVector(scaleVector) * DirectX::XMMatrixRotationQuaternion(rotationVector) * DirectX::XMMatrixTranslationFromVector(translationVector);


	// Create a new matrix with the desired scale
	//DirectX::XMMATRIX scaleMatrix = {};
	//scaleMatrix.r[0].m128_f32[0] = aScale.x;
	//scaleMatrix.r[1].m128_f32[1] = aScale.y;
	//scaleMatrix.r[2].m128_f32[2] = aScale.z;
	//scaleMatrix.r[3].m128_f32[3] = 1.0f;
	//
	//myMatrix *= scaleMatrix;
}

Vector3f Transform::GetPosition() const
{
	return Vector3f(myMatrix.r[3].m128_f32[0], myMatrix.r[3].m128_f32[1], myMatrix.r[3].m128_f32[2]);
}

Vector3f Transform::GetRotation() const
{
	//calculate a euler angle rotation vector based on the directional vectors of the matrix
	
	//Vector3f rotation;
	//rotation.x = atan2f(myMatrix.r[2].m128_f32[1], myMatrix.r[2].m128_f32[2]);
	//rotation.y = atan2f(-myMatrix.r[2].m128_f32[0], sqrtf(myMatrix.r[2].m128_f32[1] * myMatrix.r[2].m128_f32[1] + myMatrix.r[2].m128_f32[2] * myMatrix.r[2].m128_f32[2]));
	//rotation.z = atan2f(myMatrix.r[1].m128_f32[0], myMatrix.r[0].m128_f32[0]);
	//
	//rotation = RadiansToDegrees(rotation);

	//Vector3f rotation;
	//DirectX::XMFLOAT4X4 XMFLOAT4X4_Values;
	//DirectX::XMStoreFloat4x4(&XMFLOAT4X4_Values, DirectX::XMMatrixTranspose(myMatrix));
	//
	//rotation.x = (float)asin(-XMFLOAT4X4_Values._23);
	//rotation.y = (float)atan2(XMFLOAT4X4_Values._13, XMFLOAT4X4_Values._33);
	//rotation.z = (float)atan2(XMFLOAT4X4_Values._21, XMFLOAT4X4_Values._22);
	//
	//rotation = RadiansToDegrees(rotation);
	
	//Vector3f rotation;
	//float pitch = DirectX::XMScalarASin(-myMatrix.r[2].m128_f32[1]);
	//
	//DirectX::XMVECTOR from(DirectX::XMVectorSet(myMatrix.r[0].m128_f32[1], myMatrix.r[2].m128_f32[0], 0.0f, 0.0f));
	//DirectX::XMVECTOR to(DirectX::XMVectorSet(myMatrix.r[1].m128_f32[1], myMatrix.r[2].m128_f32[2], 0.0f, 0.0f));
	//DirectX::XMVECTOR res(DirectX::XMVectorATan2(from, to));
	//
	//float roll = DirectX::XMVectorGetX(res);
	//float yaw = DirectX::XMVectorGetY(res);
	//
	//rotation = { pitch, yaw, roll };
	//
	//rotation = RadiansToDegrees(rotation);

	//float sy = sqrt(myMatrix.r[0].m128_f32[0] * myMatrix.r[0].m128_f32[0] + myMatrix.r[1].m128_f32[0] * myMatrix.r[1].m128_f32[0]);
	//bool singular = sy < 1e-6; // If
	//
	//Vector3f rotation;
	//if (!singular)
	//{
	//	rotation.x = atan2(myMatrix.r[2].m128_f32[1], myMatrix.r[2].m128_f32[2]);
	//	rotation.y = atan2(-myMatrix.r[2].m128_f32[0], sy);
	//	rotation.z = atan2(myMatrix.r[1].m128_f32[0], myMatrix.r[0].m128_f32[0]);
	//}
	//else
	//{
	//	rotation.x = atan2(-myMatrix.r[1].m128_f32[2], myMatrix.r[1].m128_f32[1]);
	//	rotation.y = atan2(-myMatrix.r[2].m128_f32[0], sy);
	//	rotation.z = 0;
	//}

	//Vector3f rotation;
	//DirectX::XMVECTOR quaternion = DirectX::XMQuaternionRotationMatrix(myMatrix);
	//float qx = quaternion.m128_f32[0];
	//float qy = quaternion.m128_f32[1];
	//float qz = quaternion.m128_f32[2];
	//float qw = quaternion.m128_f32[3];
	//rotation.x = atan2(2 * (qw * qx + qy * qz), 1 - 2 * (qx * qx + qy * qy));
	//rotation.y = -asin(2 * (qw * qy - qz * qx));
	//rotation.z = atan2(2 * (qw * qz + qx * qy), 1 - 2 * (qy * qy + qz * qz));
	//
	//rotation = RadiansToDegrees(rotation);

	//Vector3f pos;
	//Quaternionf quat;
	//Vector3f scale;
	//
	//DecomposeMatrix(scale, quat, pos);
	//
	//Vector3f rotation = RadiansToDegrees(quat.GetEulerAnglesRadians());

	Vector3f rotation;
	rotation.y = asin(std::clamp(myMatrix.r[2].m128_f32[0], -1.0f, 1.0f));
	
	if (abs(myMatrix.r[2].m128_f32[0]) < 0.9999999f)
	{
		rotation.x = atan2(-myMatrix.r[2].m128_f32[1], myMatrix.r[2].m128_f32[2]);
		rotation.z = atan2(-myMatrix.r[1].m128_f32[0], myMatrix.r[0].m128_f32[0]);
	}
	else
	{
		rotation.x = atan2(myMatrix.r[0].m128_f32[2], myMatrix.r[1].m128_f32[1]);
		rotation.z = 0;
	}
	
	rotation = RadiansToDegrees(rotation);

	//float x0, x1, y0, y1, z0, z1;
	//
	//Vector3f rotation;
	//if (myMatrix.r[2].m128_f32[0] != 1 && myMatrix.r[2].m128_f32[0] != -1)
	//{
	//	y0 = -asin(myMatrix.r[2].m128_f32[0]);
	//	y1 = PI - y0;
	//
	//	x0 = atan2(myMatrix.r[2].m128_f32[1] / cos(y0), myMatrix.r[2].m128_f32[2] / cos(y0));
	//	x1 = atan2(myMatrix.r[2].m128_f32[1] / cos(y1), myMatrix.r[2].m128_f32[2] / cos(y1));
	//
	//	z0 = atan2(myMatrix.r[1].m128_f32[0] / cos(y0), myMatrix.r[0].m128_f32[0] / cos(y0));
	//	z1 = atan2(myMatrix.r[1].m128_f32[0] / cos(y1), myMatrix.r[0].m128_f32[0] / cos(y1));
	//}
	//else
	//{
	//	z0 = 0;
	//	if (myMatrix.r[2].m128_f32[0] == -1)
	//	{
	//		y0 = PI / 2.f;
	//		x0 = z0 + atan2(myMatrix.r[0].m128_f32[1], myMatrix.r[0].m128_f32[2]);
	//	}
	//	else
	//	{
	//		y0 = -PI / 2;
	//		x0 = -z0 + atan2(-myMatrix.r[0].m128_f32[1], -myMatrix.r[0].m128_f32[2]);
	//	}
	//}
	//rotation.x = RadiansToDegrees(x0);
	//rotation.y = RadiansToDegrees(y0);
	//rotation.z = RadiansToDegrees(z0);
	//if (rotation.x < 0)
	//	rotation.x += 360.f;
	//if (rotation.y < 0)
	//	rotation.y += 360.f;
	//if (rotation.z < 0)
	//	rotation.z += 360.f;

	return rotation;
}

Vector3f Transform::GetScale() const
{
	Vector3f pos;
	Quaternionf quat;
	Vector3f scale;
	DecomposeMatrix(scale, quat, pos);

	//scale.x = sqrtf(myMatrix.r[0].m128_f32[0] * myMatrix.r[0].m128_f32[0] + myMatrix.r[0].m128_f32[1] * myMatrix.r[0].m128_f32[1] + myMatrix.r[0].m128_f32[2] * myMatrix.r[0].m128_f32[2]);
	//scale.y = sqrtf(myMatrix.r[1].m128_f32[0] * myMatrix.r[1].m128_f32[0] + myMatrix.r[1].m128_f32[1] * myMatrix.r[1].m128_f32[1] + myMatrix.r[1].m128_f32[2] * myMatrix.r[1].m128_f32[2]);
	//scale.z = sqrtf(myMatrix.r[2].m128_f32[0] * myMatrix.r[2].m128_f32[0] + myMatrix.r[2].m128_f32[1] * myMatrix.r[2].m128_f32[1] + myMatrix.r[2].m128_f32[2] * myMatrix.r[2].m128_f32[2]);
	
	
	
	return scale;
}

void Transform::DecomposeMatrix(Vector3f& aScale, Quaternionf& aQuat, Vector3f& aPos) const
{
	DirectX::XMVECTOR scaleVector;
	DirectX::XMVECTOR rotationVector;
	DirectX::XMVECTOR translationVector;

	DirectX::XMMatrixDecompose(&scaleVector, &rotationVector, &translationVector, myMatrix);
	aScale.x = scaleVector.m128_f32[0];
	aScale.y = scaleVector.m128_f32[1];
	aScale.z = scaleVector.m128_f32[2];

	aPos.x = translationVector.m128_f32[0];
	aPos.y = translationVector.m128_f32[1];
	aPos.z = translationVector.m128_f32[2];

	aQuat.X = rotationVector.m128_f32[0];
	aQuat.Y = rotationVector.m128_f32[1];
	aQuat.Z = rotationVector.m128_f32[2];
	aQuat.W = rotationVector.m128_f32[3];
}

void Transform::RotateAroundX(float aAngle)
{
	myMatrix = DirectX::XMMatrixRotationX(DegreesToRadians(aAngle)) * myMatrix;
}
void Transform::RotateAroundY(float aAngle)
{
	myMatrix = DirectX::XMMatrixRotationY(DegreesToRadians(aAngle)) * myMatrix;
}
void Transform::RotateAroundZ(float aAngle)
{
	myMatrix = DirectX::XMMatrixRotationZ(DegreesToRadians(aAngle)) * myMatrix;
}
void Transform::RotateAroundWorldX(float anAngle)
{
	myMatrix = myMatrix * DirectX::XMMatrixRotationX(DegreesToRadians(anAngle));
}
void Transform::RotateAroundWorldY(float anAngle)
{
	myMatrix = myMatrix * DirectX::XMMatrixRotationY(DegreesToRadians(anAngle));
}
void Transform::RotateAroundWorldZ(float anAngle)
{
	myMatrix = myMatrix * DirectX::XMMatrixRotationZ(DegreesToRadians(anAngle));
}

void Transform::RotateAroundAxis(const Vector3f& aAxis, float aAngle)
{
	if (aAxis != aAxis)
	{
		LogError("Transform::RotateAroundAxis: Axis is NaN");
	}
	if (aAngle != aAngle)
	{
		LogError("Transform::RotateAroundAxis: Angle is Nan");
	}
	auto rotationMatrix = DirectX::XMMatrixRotationAxis(CreateDXVector(aAxis), DegreesToRadians(aAngle));
	rotationMatrix.r[3] = myMatrix.r[3];
	myMatrix.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	myMatrix = myMatrix * rotationMatrix;
}

void Transform::Rotate(DirectX::XMVECTOR aQuaternion)
{
	// Convert the quaternion rotation to a matrix
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(aQuaternion);

	// Combine the rotation matrix with the existing transformation matrix
	myMatrix *= rotationMatrix;
}

void Transform::Rotate(const Quaternionf& aQuaternion)
{
	DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(aQuaternion.X, aQuaternion.Y, aQuaternion.Z, aQuaternion.W);
	Rotate(quaternion);
}

void Transform::Rotate(const Vector4f& aQuaternion)
{
	DirectX::XMVECTOR quaternion = DirectX::XMVectorSet(aQuaternion.x, aQuaternion.y, aQuaternion.z, aQuaternion.w);
	Rotate(quaternion);
}

void Transform::Move(const Vector3f& aMovement)
{
	myMatrix.r[3] = DirectX::XMVectorAdd(myMatrix.r[3], CreateDXVector(aMovement));
}

void Transform::MoveForward(float aAmount)
{
	myMatrix.r[3] = DirectX::XMVectorAdd(myMatrix.r[3], DirectX::XMVectorScale(myMatrix.r[2], aAmount));
}

void Transform::MoveRight(float aAmount)
{
	myMatrix.r[3] = DirectX::XMVectorAdd(myMatrix.r[3], DirectX::XMVectorScale(myMatrix.r[0], aAmount));
}

void Transform::MoveUp(float aAmount)
{
	myMatrix.r[3] = DirectX::XMVectorAdd(myMatrix.r[3], DirectX::XMVectorScale(myMatrix.r[1], aAmount));
}

Vector3f Transform::GetForward() const
{
	return Vector3f(myMatrix.r[2].m128_f32[0], myMatrix.r[2].m128_f32[1], myMatrix.r[2].m128_f32[2]).GetNormalized();
}

Vector3f Transform::GetRight() const
{
	return Vector3f(myMatrix.r[0].m128_f32[0], myMatrix.r[0].m128_f32[1], myMatrix.r[0].m128_f32[2]).GetNormalized();
}

Vector3f Transform::GetUp() const
{
	return Vector3f(myMatrix.r[1].m128_f32[0], myMatrix.r[1].m128_f32[1], myMatrix.r[1].m128_f32[2]).GetNormalized();
}

void Transform::LookAt(const Vector3f& aTarget)
{
	RotateTowards(aTarget, 360.0f);

}

bool Transform::RotateTowards(const Vector3f& aTarget, float aSpeed)
{
	Vector3f forward = GetForward();
	Vector3f targetDir = (aTarget - GetPosition()).GetNormalized();

	float dot = forward.Dot(targetDir);

	if (dot > 0.9999f)
	{
		dot = 0.9999f;
	}
	else if (dot < -0.9999f)
	{
		dot = -0.9999f;
	}

	float angle = acos(dot);
	if (angle != angle)
	{
		LogError("Transform::RotateTowards: Angle is NaN");
		return false;
	}
	angle = RadiansToDegrees(angle);
	if (angle > aSpeed)
	{
		angle = aSpeed;
	}
	Vector3f cross = forward.Cross(targetDir);
	if (cross.Length() > 0.0001f)
	{
		RotateAroundAxis(cross, angle);
		if (GetPosition() != GetPosition())
		return false;
	}
	return true;
}

bool Transform::RotateTowardsAroundAxis(const Vector3f& aTarget, const Vector3f& aWorldAxis, float aSpeed)
{
	Vector3f forward = GetForward();
	Vector3f targetDir = (aTarget - GetPosition()).GetNormalized();

	// Calculate the angle between the current forward direction and the target direction
	float dot = forward.Dot(targetDir);
	if (dot > 0.9999f)
	{
		dot = 0.9999f;
	}
	else if (dot < -0.9999f)
	{
		dot = -0.9999f;
	}
	float angle = acos(dot);
	if (angle != angle)
	{
		LogError("Transform::RotateTowards: Angle is NaN");
		return false;
	}
	angle = min(angle, aSpeed);

	//Convert the axis to local space
	//DirectX::XMMATRIX worldToLocalMatrix = DirectX::XMMatrixInverse(nullptr, myMatrix);
	//Vector3f localAxis = TransformDirectionToLocal(aWorldAxis).GetNormalized();

	// Rotate around the local axis
	RotateAroundAxis(aWorldAxis, angle);

	// Check if the position has changed (this assumes RotateAroundAxis modifies the position)
	if (GetPosition() != GetPosition())
	{
		return false;
	}

	return true;
}

bool Transform::RotateTowardsAroundX(const Vector3f& aTarget, float aSpeed)
{
	return RotateTowards(Vector3f(GetPosition().x, aTarget.y, aTarget.z), aSpeed);
}

bool Transform::RotateTowardsAroundY(const Vector3f& aTarget, float aSpeed)
{
	return RotateTowards(Vector3f(aTarget.x, GetPosition().y, aTarget.z), aSpeed);
}

bool Transform::RotateTowardsAroundZ(const Vector3f& aTarget, float aSpeed)
{
	return RotateTowards(Vector3f(aTarget.x, aTarget.y, GetPosition().z), aSpeed);
}

//Vector3f Transform::GetForwardRotation() const
//{
//	Vector3f forward = GetForward();
//	float yaw = atan2(forward.y, forward.x);
//
//	// Calculate pitch (elevation) angle
//	float pitch = asin(forward.z);
//
//	// Roll angle is not uniquely determined by the direction vector.
//	// You might need additional information to determine the roll.
//
//	return { RadiansToDegrees(pitch), RadiansToDegrees(yaw), 0.0f }; // Assuming roll is 0.0f
//}

float Transform::DegreesToRadians(float degrees)
{
	return degrees * DirectX::XM_PI / 180.0f;
}
float Transform::RadiansToDegrees(float radians)
{
	return radians * (180.0f / DirectX::XM_PI);

}

Vector3f Transform::DegreesToRadians(Vector3f degrees)
{
	return Vector3f(DegreesToRadians(degrees.x), DegreesToRadians(degrees.y), DegreesToRadians(degrees.z));
}

Vector3f Transform::RadiansToDegrees(Vector3f radians)
{
		return Vector3f(RadiansToDegrees(radians.x), RadiansToDegrees(radians.y), RadiansToDegrees(radians.z));

}

Vector3f Transform::TransformDirectionToLocal(const Vector3f& directionWorld)
{
	// Inverse of the world-to-local transformation matrix gives local-to-world
	DirectX::XMMATRIX localToWorldMatrix = DirectX::XMMatrixInverse(nullptr, myMatrix);

	// Convert the world direction vector to XMVECTOR
	DirectX::XMVECTOR directionWorldVector = DirectX::XMVectorSet(directionWorld.x, directionWorld.y, directionWorld.z, 0.0f);

	// Transform the direction vector from world to local space
	DirectX::XMVECTOR directionLocalVector = DirectX::XMVector3TransformNormal(directionWorldVector, localToWorldMatrix);

	// Convert the transformed direction back to Vector3f
	Vector3f directionLocal(DirectX::XMVectorGetX(directionLocalVector), DirectX::XMVectorGetY(directionLocalVector), DirectX::XMVectorGetZ(directionLocalVector));

	return directionLocal;
}