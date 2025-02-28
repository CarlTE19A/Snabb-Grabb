#include "stdafx.h"
#include "triggerZoneComponent.h"
#include "gameObject\GameObject.h"

#include "mainSingelton\MainSingelton.h"
#include "events\inputMapper\InputMapper.h"

#include "tools\lineDrawer\LineDrawer.h"
#include "math\Ray.h"
#include "math/Intersection.h"

#include "scene\Scene.h"

TriggerZoneComponent::TriggerZoneComponent() : myObserver(Message::Type::KeyAdded, std::bind(&TriggerZoneComponent::AddKey, this, std::placeholders::_1))
{
	myState = eInActive;
	myActivator = eNone;
	keyCountMax = 0;
	keyCountCurrent = 0;

	auto& inputMapper = MainSingleton::GetInstance().GetInputMapper();
	inputMapper.AddObserver(eInputEvent::eLeftMouseClick, this);
}

TriggerZoneComponent::~TriggerZoneComponent()
{
	for (Action* action : myActions)
	{
		delete action;
	}
	myActions.clear();
	for (size_t i = 0; i < maxActionOrder; i++)
	{
		for (Action* action : myUnFinishedActions[i])
		{
			delete action;
		}
	}

	auto& inputMapper = MainSingleton::GetInstance().GetInputMapper();
	inputMapper.RemoveObserver(eInputEvent::eLeftMouseClick, this);
}

void TriggerZoneComponent::Update(float aDeltaTime, Scene& aScene)
{
	aScene;
	switch (myState)
	{
	case eInActive:
		if (keyCountCurrent == 0)
		{
			TestActivate(aScene);
		}
		return;
	case eActive:
		
		for (int i = 0; i < myUnFinishedActions[currentActionOrder].size(); i++)
		{
			if (myUnFinishedActions[currentActionOrder][i]->Update(aDeltaTime, aScene))
			{
				myActions.push_back(myUnFinishedActions[currentActionOrder][i]);
				myUnFinishedActions[currentActionOrder].erase(myUnFinishedActions[currentActionOrder].begin() + i);
				i--;
			}
		}
		if (myUnFinishedActions[currentActionOrder].size() == 0)
		{
			currentActionOrder++;
			if (currentActionOrder < maxActionOrder)
			{
				for (int i = 0; i < myUnFinishedActions[currentActionOrder].size(); i++)
				{
					myUnFinishedActions[currentActionOrder][i]->Activate(aScene);
				}
			}
		}
		if (currentActionOrder == maxActionOrder)
		{
			myState = eDone;
			LogInfo("TriggerZoneComponent: Done");
		}
		
		return;
	case eDone:
		if (isRepeatable == true)
		{
			myState = eInActive;

		}
		//Do nothing
		return;
	default:
		return;
	}
}

void TriggerZoneComponent::TestActivate(Scene&)
{
	return;
}

void TriggerZoneComponent::Reset()
{
}

void TriggerZoneComponent::AddAction(Action* aAction)
{
	myActions.push_back(aAction);
}

void TriggerZoneComponent::SetActivator(TriggerActivator aActivator)
{
	myActivator = aActivator;
}

void TriggerZoneComponent::SetActorType(TriggerActor aActorType)
{
	myActorType = aActorType;
}

void TriggerZoneComponent::SetRepeatable(bool aRepeatable)
{
	isRepeatable = aRepeatable;
}

void TriggerZoneComponent::AddLock()
{
	keyCountMax++;
	keyCountCurrent++;
}

void TriggerZoneComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Triggerzone"))
	{
		ImguiNativeUpdate();
		std::string activatorString = "";
		switch (myActivator)
		{
		case eNone:
			activatorString = "None";
			break;
		case eOnEnter:
			activatorString = "OnEnter";
			break;
		case eOnClick:
			activatorString = "OnClick";
			break;
		case eOnExit:
			activatorString = "OnExit";
			break;
		case eOnUnlock:
			activatorString = "OnUnlock";
			break;
		default:
			activatorString = "Error";
			break;
		}
		std::string stateString = "";
		switch (myState)
		{
			case eInActive:
				stateString = "InActive";
				break;
			case eActive:
				stateString = "Active";
				break;
			case eDone:
				stateString = "Done";
				break;
		}
		ImGui::Text(((std::string)"Activator: " + activatorString).c_str());
		ImGui::Text(((std::string)"State: " + stateString).c_str());
		ImGui::Text("KeyCountMax: %i", keyCountMax);
		ImGui::Text("KeyCountCurrent: %i", keyCountCurrent);

		//if (ImGui::Button("Activate"))
		//{
		//	//ActivateActions(scene);
		//}
		if (ImGui::Button("Reset"))
		{
			ResetActions();
		}
		if (ImGui::Button("Add Fake Lock"))
		{
			AddLock();
		}
		if (ImGui::Button("Add Fake Key"))
		{
			AddKey(Message(Message::Type::KeyAdded, myParent->GetID()));
		}
		ImGui::EndTabItem();
	};
}

void TriggerZoneComponent::AddKey(Message aMessage)
{
	int lockID = std::any_cast<int>(aMessage.myData);
	if (lockID == myParent->GetID())
	{
		if (keyCountCurrent > 0)
		{
			keyCountCurrent--;
		}
		else
		{
			LogError("TriggerZoneComponent: KeyCountCurrent is already 0 " + std::to_string(myParent->GetID()));
		}
	}
}

void TriggerZoneComponent::ActivateActions(Scene& aScene)
{
	if (myState != eActive)
	{
		myState = eActive;
		currentActionOrder = 0;


		for(Action* action : myActions)
		{
			myUnFinishedActions[action->order].push_back(action);
		}
		myActions.clear();
		for (int i = 0; i < myUnFinishedActions[0].size(); i++)
		{
			myUnFinishedActions[0][i]->Activate(aScene);
		}
	}
}

void TriggerZoneComponent::ResetActions()
{
	if (myState != eInActive)
	{
		myState = eInActive;
		for(int i = 0; i < maxActionOrder; i++)
		{
			for (Action* action : myUnFinishedActions[i])
			{
				myActions.push_back(action);
			}
			myUnFinishedActions[i].clear();
		}
		for (int i = 0; i < myActions.size(); i++)
		{
			myActions[i]->Reset();
		}
	}
}

void TriggerZoneComponent::RecieveEvent(const eInputEvent aEvent, std::any aValue)
{
	if (aEvent == eInputEvent::eLeftMouseClick)
	{
		IsClicked();
	}
}

void TriggerZoneComponent::IsClicked()
{
	return;
}
