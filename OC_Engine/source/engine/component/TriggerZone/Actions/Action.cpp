#include "stdafx.h"
#include "Action.h"
#include "mainSingelton\MainSingelton.h"
#include "events/EventSystem.h"
#include "component\TriggerZone\triggerZoneComponent.h"
#include "gameObject\GameObject.h"

void Action::ActivateKeys()
{
	EventSystem& eventSys = MainSingleton::GetInstance().GetEventSystem();
	for(int key : myKeyTargets)
	{
		eventSys.SendMsg(Message(Message::Type::KeyAdded, key));
	}
}

void Action::AddKeyTarget(TriggerZoneComponent* aTriggerZoneTarget)
{
	myKeyTargets.push_back(aTriggerZoneTarget->GetOwner()->GetID());
	aTriggerZoneTarget->AddLock();
}
