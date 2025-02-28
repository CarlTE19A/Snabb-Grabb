#pragma once
#include "Action.h"
class GameObject;

class MoveTransformAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	~MoveTransformAction() override;


	inline void SetOffset(const Vector3f& aOffset) { myOffset = aOffset; }
	//inline void SetStartPosition(const Vector3f& aPosition) { myStartPosition = aPosition; }
	//inline void SetEndPosition(const Vector3f& aPosition) { myEndPosition = aPosition; }
	inline void SetTime(float aTime) { myTime = aTime; }
	inline void SetTarget(GameObject* aTarget) { target = aTarget; }
private:
	Vector3f myStartPosition;
	Vector3f myOffset;
	Vector3f myEndPosition;
	float myTime;
	GameObject* target;
	float percentage = 0;
};