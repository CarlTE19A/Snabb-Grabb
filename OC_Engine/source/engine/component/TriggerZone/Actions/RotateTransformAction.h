#pragma once
#include "Action.h"
class GameObject;

class RotateTransformAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	~RotateTransformAction();


	inline void SetRotation(const Vector3f& aRotation) { myEulerRotation = aRotation; }
	inline void SetTime(float aTime) { myTime = aTime; }
	inline void SetTarget(GameObject* aTarget) { target = aTarget; }

private:
	Vector3f myEulerRotation;
	float myTime;
	GameObject* target;
	float percentage = 0;
	float lastPercentage = 0;
};