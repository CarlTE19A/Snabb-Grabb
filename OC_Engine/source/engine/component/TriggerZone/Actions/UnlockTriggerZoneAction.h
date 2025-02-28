#pragma once
#include "Action.h"

class UnlockTriggerZoneAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	~UnlockTriggerZoneAction() override;

private:
};