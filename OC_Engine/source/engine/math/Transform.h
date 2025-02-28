#pragma once
#include "Vector.h"
#include "Quaternion.h"
#include <DirectXMath.h>

#include "PhysX\foundation/Px.h"
#include "PhysX\foundation/PxMat44.h"

//namespace physx
//{
//	class PxMat44;
//}

class Transform
{
private:
	DirectX::XMMATRIX myMatrix;
	void ResetRotationAndScale();
	Vector3f TransformDirectionToLocal(const Vector3f& directionWorld);
public:
	Transform();

	inline DirectX::XMMATRIX& GetMatrixReference() { return myMatrix; }
	inline DirectX::XMMATRIX GetMatrix() const { return myMatrix; }

	void ResetMatrix();

	inline void SetMatrix(DirectX::XMMATRIX aMatrix) { myMatrix = aMatrix; }
	//Sets the matrix from a PxMat44, used for PhysX
	void SetMatrixFromPxMat44(const physx::PxMat44& aMatrix);
	//Sets the matrix from a PxMat44, used for PhysX
	physx::PxMat44 GetPxMat44();

	void SetPosition(const Vector3f& aPosition);
	//DEPRECATED - Do not directly set rotations, use RotateAround or RotateTowards
	void SetRotation(const Vector3f& aRotation);
	void SetScale(const Vector3f& aScale);

	Vector3f GetPosition() const;
	//Gives a Euler Angle, is NOT be safe to count on for rotations
	Vector3f GetRotation() const;
	Vector3f GetScale() const;
	void DecomposeMatrix(Vector3f& aScale, Quaternionf& aRotation, Vector3f& aPosition) const;

	void RotateAroundX(float aAngle);
	void RotateAroundY(float aAngle);
	void RotateAroundZ(float aAngle);

	void RotateAroundWorldX(float anAngle);
	void RotateAroundWorldY(float anAngle);
	void RotateAroundWorldZ(float anAngle);

	void RotateAroundAxis(const Vector3f& aAxis, float aAngle);

	void Rotate(DirectX::XMVECTOR aQuaternion);
	void Rotate(const Quaternionf& aQuaternion);
	void Rotate(const Vector4f& aQuaternion);

	void Move(const Vector3f& aMovement);
	void MoveForward(float aAmount);
	void MoveRight(float aAmount);
	void MoveUp(float aAmount);

	Vector3f GetForward() const;
	Vector3f GetRight() const;
	Vector3f GetUp() const;

	//Instantly look at a target
	void LookAt(const Vector3f& aTarget);
	//Rotate towards a target with a max degree per frame
	bool RotateTowards(const Vector3f& aTarget, float aSpeed);
	//Rotate towards a target with a max degree per frame ignoring the X position
	bool RotateTowardsAroundX(const Vector3f& aTarget, float aSpeed);
	//Rotate towards a target with a max degree per frame ignoring the Y position
	bool RotateTowardsAroundY(const Vector3f& aTarget, float aSpeed);
	//Rotate towards a target with a max degree per frame ignoring the Z position
	bool RotateTowardsAroundZ(const Vector3f& aTarget, float aSpeed);
	//Vector3f GetForwardRotation() const;
	bool RotateTowardsAroundAxis(const Vector3f& aTarget, const Vector3f& aAxis, float aSpeed);

	static float DegreesToRadians(float aDegrees);
	static float RadiansToDegrees(float aRadians);

	static Vector3f DegreesToRadians(const Vector3f aDegrees);
	static Vector3f RadiansToDegrees(const Vector3f aRadians);
};