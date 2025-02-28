#pragma once
#include "Action.h"
class Component;

enum class eToggleAction
{
	Enable = 0,
	Disable = 1,
	Toggle = 2,
};

class ToggleTriggerZoneAction : public Action
{
public:
	void Activate(Scene& aScene) override;
	void Reset() override;
	bool Update(float deltaTime, Scene& aScene) override;
	~ToggleTriggerZoneAction();

	void SetTarget(Component* aTarget);
	void SetToggleAction(eToggleAction aToggleAction);

private:
	Component* myTarget;
	eToggleAction myToggleAction;

};