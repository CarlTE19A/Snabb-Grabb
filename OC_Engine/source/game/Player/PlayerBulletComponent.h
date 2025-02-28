#pragma once
#include "component/Component.h"

class PlayerBulletComponent : public Component
{
public:
	PlayerBulletComponent();
	~PlayerBulletComponent();
	float Start(Vector3f aStartPos, Vector3f aEndPos, float aSpeed);
	void Update(const float aDeltaTime, Scene&)override;
	void ImguiUpdate() override;
	void DeleteMe(Scene& aActiveScene);
	void Reset() override;

private:
	bool myIsPlaying = false;
	float timer = 0;
	//Vector3f myStart{};
	//Vector3f myEnd{};
	//float mySpeed = 0;
	float completion = 0;
	float completionTime = 0;
	Vector3f movePerSecond{};
};