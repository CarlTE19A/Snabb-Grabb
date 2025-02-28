#pragma once
#include "Component.h"


class RotatorComponent : public Component
{
private:
	Vector3f myRotation;

public:
	RotatorComponent();
	~RotatorComponent() override;

	void Update(const float aDeltaTime, Scene& aScene) override;

	void ImguiUpdate();
	inline void SetRotator(Vector3f rotator){ myRotation = rotator; }
	
	void Reset(){};
};