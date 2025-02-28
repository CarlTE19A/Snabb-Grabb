#include "stdafx.h"
#include "UnlockTriggerZoneAction.h"

void UnlockTriggerZoneAction::Activate(Scene&)
{
	ActivateKeys();
	myActive = false;
}

void UnlockTriggerZoneAction::Reset()
{
}

bool UnlockTriggerZoneAction::Update(float deltaTime, Scene& aScene)
{
	deltaTime;
	aScene;
	return true;
}

UnlockTriggerZoneAction::~UnlockTriggerZoneAction()
{
}
