#include "stdafx.h"
#include "DelayAction.h"

#include "scene/Scene.h"

void DelayAction::Activate(Scene&)
{
	myTimer = 0.f;
}

void DelayAction::Reset()
{
}

bool DelayAction::Update(float deltaTime, Scene& aScene)
{
	if (myTimer < myDelay)
	{
		myTimer += deltaTime;
		return false;
	}
	ActivateKeys();
	return true;
	deltaTime;
	aScene;
}

DelayAction::~DelayAction()
{
}
