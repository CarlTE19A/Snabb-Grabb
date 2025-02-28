#pragma once
#include "../Component.h"
#include "events\EventSystem.h"
#include "events\inputMapper\InputObserver.h"
#include "Actions\Action.h"

//Make sure these enums are match with Unity's enums in TriggerZone.cs
#define maxActionOrder 10

enum TriggerActivator
{
	eNone		= -1,
	eOnEnter	= 0,
	eOnClick	= 1,
	eOnExit		= 2,
	eOnUnlock	= 3,
};

enum TriggerActor
{
	ePlayer = 0,
	eEnemy = 1,
	eBoth = 2,
};

enum TriggerState
{
	eInActive,
	eActive,
	eDone
};

class TriggerZoneComponent : public Component, public InputObserver
{
public:
	TriggerZoneComponent();
	~TriggerZoneComponent() override;

	void Update(float aDeltaTime, Scene& aScene) override;
	void TestActivate(Scene& aScene);

	void Reset() override;

	void AddAction(Action* aAction);
	void SetActivator(TriggerActivator aActivator);
	void SetActorType(TriggerActor aActorType);
	void SetRepeatable(bool aRepeatable);
	void AddLock();
	void ImguiUpdate() override;
private:
	std::vector<Action*> myActions;
	std::vector<Action*> myUnFinishedActions[maxActionOrder];
	TriggerActivator myActivator;
	TriggerActor myActorType;
	TriggerState myState;

	void AddKey(Message aMessage);
	void ActivateActions(Scene& aScene);
	void ResetActions();

	int keyCountMax;
	int keyCountCurrent;
	Observer myObserver;

	int currentActionOrder = 0;

	//Only used by eOnExit
	bool isInside = false;

	//Only used by eOnClick
	void RecieveEvent(const eInputEvent aEvent, std::any aValue) override;
	void IsClicked();
	bool isClicked = false;
	bool isRepeatable = false;
};