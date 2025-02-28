#pragma once
#include "Action.h"

class DelayAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	~DelayAction();

	void SetDelay(float aDelay) { myDelay = aDelay; }

private:
	float myDelay;
	float myTimer;
};