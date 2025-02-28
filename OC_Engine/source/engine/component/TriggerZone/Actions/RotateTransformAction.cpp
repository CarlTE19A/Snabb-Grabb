#include "stdafx.h"
#include "RotateTransformAction.h"
#include "gameObject/GameObject.h"
#include "scene/Scene.h"

void RotateTransformAction::Activate(Scene&)
{
	percentage = 0;
	lastPercentage = 0;
}

void RotateTransformAction::Reset()
{
}

bool RotateTransformAction::Update(float deltaTime, Scene& aScene)
{
	percentage += deltaTime / myTime;
	if (percentage >= 1)
	{
		percentage = 1;
	}
	target->GetTransform()->RotateAroundX(myEulerRotation.x * (percentage - lastPercentage));
	target->GetTransform()->RotateAroundY(myEulerRotation.y * (percentage - lastPercentage));
	target->GetTransform()->RotateAroundZ(myEulerRotation.z * (percentage - lastPercentage));
	if (percentage >= 1)
	{
		ActivateKeys();
		myActive = false;
		return true;
	}
	lastPercentage = percentage;
	return false;
	aScene;
}

RotateTransformAction::~RotateTransformAction()
{
}
