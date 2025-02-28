#include "stdafx.h"
#include "ToggleTriggerZoneAction.h"

#include "scene/Scene.h"

#include "component\Component.h"

void ToggleTriggerZoneAction::Activate(Scene&)
{
	if (myTarget != nullptr)
	{
		switch (myToggleAction)
		{
		case eToggleAction::Enable:
			myTarget->SetActive(true);
			break;
		case eToggleAction::Disable:
			myTarget->SetActive(false);
			break;
		case eToggleAction::Toggle:
			myTarget->SetActive(!myTarget->IsActive());
			break;
		default:
			break;
		}
	}
	else
	{
		LogWarning("ToggleTriggerZoneAction: No target set");
	}
	ActivateKeys();
}

void ToggleTriggerZoneAction::Reset()
{
}

bool ToggleTriggerZoneAction::Update(float, Scene&)
{
	return true;
}

ToggleTriggerZoneAction::~ToggleTriggerZoneAction()
{
}

void ToggleTriggerZoneAction::SetTarget(Component* aTarget)
{
	myTarget = aTarget;
}

void ToggleTriggerZoneAction::SetToggleAction(eToggleAction aToggleAction)
{
	myToggleAction = aToggleAction;
}
